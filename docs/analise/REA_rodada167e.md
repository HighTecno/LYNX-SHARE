# REA round 167e — flaky tests, performance and robustness leftovers (perf167, v0.1.22)

Brief: flaky tests (VfxScript165 endless-loop p95, `vfx_script_late_answers_play_at_the_next_block`, natives142
FL Keys silent), VFX Script export tail, Contrary Motion blip, `--visualtest` on silent starts, PCM cache size limit,
audio device opened in parallel with the main window, start-up / heavy-project profiling with before/after numbers.

Evidence markers: **[EXEC]** measured here, **[DECOMP]** Ghidra/objdump of FL's binaries, **[CODE]** our source,
**[model]** our reading. Raw data: `tmp/perf167` (start-up traces, jsonl), `E:\lynx-work\perf167` (renders, profiles).

Every item is LYNX-internal (tests, robustness, speed) except two FL-parity ones: the VFX Script export tail and the
Contrary Motion blip. No issue number; no reply drafts.

## 1. Flaky tests

### 1a. `vfx_script_errors_are_reported_and_never_hang` (endless loop, "p95 < 2.5")

- Reproduced [EXEC]: main's test binary, the VFX tests pinned to two cores shared with four busy loops
  (`taskset -c 9-10`): 3 runs → p95 7.99 / 0.09 / 9.54 ms, 2 of 3 failed. Unloaded: p95 0.01–0.04 ms, pass.
- Root cause [CODE]: once the script was stopped ("took too long"), `Runtime::process` still handed every audio block
  to the Python thread and spun (with `yield()`) up to the 2 ms budget for its empty answer. On a busy machine the
  Python thread wakes late, and `yield()` hands the core to the other processes for a whole time slice: the test
  thread came back 4–10 ms later. The bound was right; LYNX made the audio thread wait for nothing.
- Fix: a script that stopped (error, too slow, too much memory) or never compiled is *halted*: after the block in
  which that happened the Python thread sets `halted`, and the audio thread then returns at once (no job, no wait).
  A compile / Restart clears it before the next block (the voices it ended still get their note-offs).
  Real-time benefit in the app too: a broken VFX Script costs the audio thread nothing.
- Now [EXEC]: same contention, 3 of 3 pass, p95 0.00 ms. New test
  `perf167_stopped_vfx_script_costs_the_audio_thread_nothing` (5000 blocks of a stopped script: 0.04 ms in all).

### 1b. `vfx_script_late_answers_play_at_the_next_block`

- Reproduced [EXEC]: same contention, 3 of 3 failed (`first.size() == 0` and the second block's check).
- Root cause: the test's margins, not LYNX. The script took 6 ms against the 2 ms budget and the test looked again
  after a fixed 30 ms. Preempted inside its own wait for longer than the script, the test thread found the answer
  already there (correct: an answer in time plays at its sample); a Python thread starved for > 30 ms answered after
  the second look. Neither is a LYNX bug: a late answer must play at the start of the first block after it.
- Fix (the bounds were wrong): the script takes 40 ms (20× the budget, far inside the 250 ms limit) and the test
  keeps playing 5 ms blocks until the answer comes (at most 2 s), then checks it plays at offset 0.

### 1c. A third one found on the way: `vfx_script_memory_runaway_is_stopped`

- [EXEC] Under heavier contention (two cores, ten busy loops) it failed 1 of 2: "Stopped: the script took too long"
  instead of "more than 768 MB" (4.3 s wall for 0.19 s of user time).
- Root cause [CODE]: the Python time limit (export 3 s, real time 250 ms) was wall-clock time. On a busy machine a
  good script could be stopped as "too slow" in an export — a user-facing bug, not only a test one.
- Fix: the limit counts the Python thread's own CPU time (`ThreadCpuClock`: `pthread_getcpuclockid` on Linux,
  `thread_info` on macOS, `GetThreadTimes` on Windows), with 4× the limit of wall time as a backstop for a script
  that waits. The endless-loop test now waits for the stop (up to 3 s) instead of assuming it came within its 400
  blocks.

### 1d. natives142 "FL Keys silent"

- The only record: one full run on 2026-10-29 (`silent: FL Keys (0)`, RMS exactly 0) on a heavily loaded machine
  with faulty RAM. [EXEC] Not reproducible: the test alone 2× unloaded, 3× on two cores with four busy loops — pass.
- RMS 0 means the FL Keys channel had no wavetable: `flKeysWavetable` returns none when any of its 30 piano files
  (read from the FL install on /mnt/c) fails to read. Most likely a failed read of one file in that run.
- Done: the test now says, when an engine is silent, which of its files does not read now and whether a second
  render plays — so the next occurrence names the cause. Not loosened.

## 2. VFX Script: the export tail (FL parity)

- FL [EXEC, renders of round 165d: `D:\lynx-work\vfxscript165\fl\*.wav`, FL64.exe /R of the 12 cases]: every FL render
  ends at the song's end (+5 ms): Tutorial 3's pulse (a note every 1/8 while `vfx.context.isPlaying`) has no note
  after 4.0 s. Tutorial 3's own text gates generation on `isPlaying` [FL factory script]. So in FL's remainder the
  song is stopped for the script.
- LYNX before [EXEC]: the export's tail ran with the transport playing: tut3 went on pulsing every 1/8 note
  until the tail's 6 s maximum (a 10.0 s file against FL's 4.0 s).
- LYNX now: the export tells VFX Script when the music has ended (`vfx::script::setSongEnded`); in the tail the
  scripts see `isPlaying` False. Only VFX Script (the FL evidence is about it); hosted plugins keep their transport.
  Test `perf167_vfx_script_sees_the_song_stopped_in_an_export_tail`.

## 3. Contrary Motion "blip" (FL parity) — measured again, not changed

- Round 165d's note-detector (8192-sample frames) saw a one-semitone-lower note for ~35 ms at each release in FL's
  render. [EXEC] A finer look (5 ms RMS + zero-crossing pitch, `tmp/perf167`): there is no second voice; FL's sound
  ends ~10 ms sooner than LYNX's at every release, *also in Tutorial 1* (pure pass-through): FL −24 dB at 0.755 s and
  silent at 0.760, LYNX −23 dB at 0.760 and silent at 0.770. The pitch moves during FL's short decay (the detector's
  wide frame turned that into a "lower note").
- So the difference is in the release of the instrument behind the script (Sytrus in a Patcher), the same with and
  without Contrary Motion, not in the script. Left open: it needs FL's note-off timing of Patcher → Sytrus measured on
  its own (a render without VFX Script), outside this round.

## 4. `--visualtest` on a project with a silent start

- Before: the check required the worker-thread export's first sound within 0.1 s (right for the default demo); a
  project starting later (FL's ZGE demo "Astes - LIFE": 1.2 s) reported result=ERROR.
- Now: the reference is the same export run on the message thread (round 164c's bug lost the start only on another
  thread); the two first sounds must agree within 10 ms. Report line: `first sound at X s (message thread: Y s)`.

## 5. PCM cache size limit

- Before: the decoded copies (`LYNX-cache\*.L.pcm / *.R.pcm`, memory-mapped streaming of long samples) had no limit
  (2.4 GB on this PC, round 165i). Only the stretched copies had one (1 GB).
- Now: 2 GB; past it, the least recently used are deleted down to 3/4 (1.5 GB), once per session on the cache
  writer's background thread. A copy found again is marked used (its date, at most once an hour). A copy mapped now
  is safe: Windows refuses the delete, and on Linux/macOS the mapping keeps the data. The budget logic is shared
  with the stretch cache (`core/CacheBudget.h`), test `perf167_cache_budget_prunes_the_least_recently_used`.

## 6. Start-up: the audio device and the main window

(see §8 for the numbers)

## 7. Heavy project: CPU per block and allocations on the audio thread

(see §8)

## 8. Numbers

(filled in below)

## Where I stopped

- 2026-10-31 13:50 — done in the worktree (WIP pushed to wip/perf167): §1a–1c fixes, §2, §4, §5, the FL Keys
  diagnostics, renderwav `LYNX_ALLOCS=1` (allocation counter). Building tests + renderwav; next: allocation hunt on
  Thief / Exoplanet, numbers, build-local with FL Data, Windows build, land.
