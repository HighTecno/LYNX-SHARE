# REA round 167e — flaky tests, performance and robustness leftovers (perf167, v0.1.22)

Brief: flaky tests (VfxScript165 endless-loop p95, `vfx_script_late_answers_play_at_the_next_block`, natives142
FL Keys silent), VFX Script export tail, Contrary Motion blip, `--visualtest` on silent starts, PCM cache size limit,
audio device opened in parallel with the main window, start-up / heavy-project profiling with before/after numbers.

Evidence markers: **[EXEC]** measured here, **[DECOMP]** Ghidra/objdump of FL's binaries, **[CODE]** our source,
**[model]** our reading.

## Items

(filled in as each item lands)

## Where I stopped

- 2026-10-31 12:50 — worktree `lynx-wt-perf167` (feature/perf167) from origin/main 72339935. Building the test
  binary (tests-only script in the agent scratchpad) to reproduce the flaky VFX Script tests under load. Reading
  `src/core/VfxScript.cpp` (`process`: the audio thread posts a job per block and spins up to the 2 ms budget even for
  a script that is already stopped) and `src/core/Python.cpp` (`wake()` = semaphore release of the Python host thread).
