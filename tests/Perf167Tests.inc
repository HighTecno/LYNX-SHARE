// Round 167e: flaky tests, performance and robustness leftovers (docs/analise/REA_rodada167e.md)

#include "core/CacheBudget.h"

TEST (perf167_cache_budget_prunes_the_least_recently_used)
{
    using lynx::cachebudget::Entry;
    // within the limit: nothing goes
    CHECK (lynx::cachebudget::toPrune ({ { 400, 1 }, { 500, 2 } }, 1000).empty());
    // above it: the oldest first, down to 3/4 of the limit (750): 1700 - 600 (t=1) - 300 (t=2) = 800 > 750, - 200 (t=3)
    const std::vector<Entry> files { { 200, 3 }, { 600, 1 }, { 400, 9 }, { 300, 2 }, { 200, 7 } };
    const auto gone = lynx::cachebudget::toPrune (files, 1000);
    CHECK (gone == (std::vector<std::size_t> { 1, 3, 0 }));
    std::int64_t left = 0;
    for (std::size_t i = 0; i < files.size(); ++i)
        if (std::find (gone.begin(), gone.end(), i) == gone.end())
            left += files[i].bytes;
    CHECK (left == 600 && left <= 750);
    // the most recently used one stays even when it alone is above the limit's 3/4
    const auto big = lynx::cachebudget::toPrune ({ { 900, 1 }, { 900, 5 } }, 1000);
    CHECK (big == (std::vector<std::size_t> { 0, 1 }) || big == (std::vector<std::size_t> { 0 }));
    CHECK (! big.empty() && big[0] == 0);
}

TEST (perf167_vfx_script_sees_the_song_stopped_in_an_export_tail)
{
    using namespace lynx::vfx;
    // FL's Tutorial 3 pulse (a note every 1/8 while the song plays): FL's render has none after the song's end
    const char* src = R"(
import flvfx as vfx
def onTick():
  if vfx.context.isPlaying and vfx.context.ticks % 48 == 1:
    v = vfx.Voice()
    v.note = 60
    v.length = 24
    v.trigger()
)";
    auto p = makeProcessor (Kind::script, vfxs165::state (src));
    const auto playing = vfxs165::play (*p, {}, 44100);
    CHECK (vfxs165::keysOn (playing).size() >= 3);
    script::setSongEnded (true);
    auto q = makeProcessor (Kind::script, vfxs165::state (src));
    const auto tail = vfxs165::play (*q, {}, 44100);
    script::setSongEnded (false);
    CHECK (vfxs165::keysOn (tail).empty());
}

TEST (perf167_stopped_vfx_script_costs_the_audio_thread_nothing)
{
    using namespace lynx::vfx;
    // a script that stopped (here: it doesn't compile) is not handed blocks: no wait for the Python thread
    auto p = makeProcessor (Kind::script, vfxs165::state ("import flvfx as vfx\ndef onTick(:\n    pass\n"));
    script::setRealtime (true);
    Transport t;
    t.sampleRate = 44100.0;
    t.playing = true;
    EventList out;
    p->process (nullptr, 0, 512, t, out); // (the first block learns that it answers nothing)
    std::this_thread::sleep_for (std::chrono::milliseconds (20));
    p->process (nullptr, 0, 512, t, out);
    // 5000 blocks: each was a round trip to the Python thread (its wake-up: 20-60 us, up to the 2 ms budget on a busy
    // machine), at least 100 ms in all
    const auto begin = std::chrono::steady_clock::now();
    for (int b = 0; b < 5000; ++b)
    {
        auto e = vfx164::on (60);
        p->process (&e, 1, 512, t, out);
    }
    const auto ms = std::chrono::duration<double, std::milli> (std::chrono::steady_clock::now() - begin).count();
    script::setRealtime (false);
    std::printf ("  stopped script: 5000 blocks in %.3f ms\n", ms);
    CHECK (out.size() == 0);
    CHECK (ms < 60.0); // (well under 1 ms in all now)
}
