// Core tests (no JUCE). Usage: lynx_tests [folder-with-flp-files]

#include <optional>
#include <atomic>
#include <chrono>
#include <thread>
#include <random>
#include <cctype>
#include <bit>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <functional>
#include <iterator>
#include <string>
#include <tuple>
#include <vector>

#if defined(__SSE__) || defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__) || defined(_M_ARM64) // x86 SSE, or ARM64 through core/Simd.h (sse2neon)
#include "core/Simd.h"
#endif

#include "core/AutomationDisplay.h"
#include "core/ShownEffectSlot.h"
#include "core/Dither.h"
#include "core/DrumSynthLive.h"
#include "core/Effects.h"
#include "core/MixSuite.h"
#include "core/EmberClassic.h"
#include "core/Effector.h"
#include "core/FruityDelayBank.h"
#include "core/FruityMultiband.h"
#include "core/Denormals.h"
#include "core/TestProfile.h"
#include "core/FlpImport.h"
#include "core/MidiControls.h"
#include "core/MidiImport.h"
#include "core/ChannelMiscFx.h"
#include "core/FlEffects.h"
#include "core/ProjectContents.h"
#include "core/FlPatcher.h"
#include "core/Patcher.h"
#include "core/Layer.h"
#include "core/XyController.h"
#include "core/ControlSurface.h"
#include "core/VoltageController.h"
#include "core/FlSamplers.h"
#include "core/FlSynths.h"
#include "core/Inflate.h"
#include "core/WavPack.h"
#include "core/SampleFiles.h"
#include "core/FlWrapper.h"
#include "core/FlPluginDb.h"
#include "core/MissingFiles.h"
#include "core/PianoRollTools.h"
#include "core/EventEdit.h"
#include "core/Preferences.h"
#include "core/MidiSync.h"
#include "core/History.h"
#include "core/Gms.h"
#include "core/PoiZone.h"
#include "core/MiniSynth.h"
#include "core/SimSynth.h"
#include "core/Granulizer.h"
#include "core/WaveTraveller.h"
#include "core/Drumaxx.h"
#include "core/Wasp.h"
#include "core/PluginLayout.h"
#include "core/Plucked.h"
#include "core/BeepMap.h"
#include "core/Sawer.h"
#include "core/Slayer.h"
#include "core/Toxic.h"
#include "core/Harmless.h"
#include "core/Harmor.h"
#include "core/Morphine.h"
#include "core/Ogun.h"
#include "core/BassDrum.h"
#include "core/FruitKick.h"
#include "core/IppCos.h"
#include "core/Dx10.h"
#include "core/Kepler.h"
#include "core/ThreeOsc.h"
#include "core/FlResampler.h"
#include "core/FlKeys.h"
#include "core/DirectWave.h"
#include "core/OggVorbis.h"
#include "core/FlVoiceProcessor.h"
#include "core/TransistorBass.h"
#include "core/HarmlessFx.h"
#include "core/MaximusFx.h"
#include "core/EmphasisFx.h"
#include "core/VisualFx.h"
#include "core/EmphasizerFx.h"
#include "core/FlLegacyFx.h"
#include "core/FreqShifterFl.h"
#include "core/LowLifterFl.h"
#include "core/Reeverb2.h"
#include "core/LuxeVerb.h"
#include "core/Denormals.h"
#include "core/FruityDynamics.h"
#include "core/FruityShapers.h"
#include "core/ParametricEqFl.h"
#include "core/FruityBasicFx.h"
#include "core/EquoFx.h"
#include "core/DistructorFx.h"
#include "core/MultibandDelayFx.h"
#include "core/PitchShifterFl.h"
#include "core/FruityFilters.h"
#include "core/NewtoneFx.h"
#include "core/FruityDelay.h"
#include "core/StereoFx.h"
#include "core/FrequencySplitter.h"
#include "core/ChorusFx.h"
#include "core/GrossBeat.h"
#include "core/LovePhilter.h"
#include "core/FlangusFastDist.h"
#include "core/FruityPhaser.h"
#include "core/FruityVocoder.h"
#include "core/DelphiMath.h"
#include "core/LiveEventQueue.h"
#include "core/RackRenderer.h"
#include "core/MetronomeVoice.h"
#include "engine/Transport.h"
#include "core/Patcherize.h"
#include "core/Performance.h"
#include "core/Scheduler.h"
#include "core/MarkerPlan.h"
#include "core/SnapshotExchange.h"
#include "core/SoftClip.h"
#include "core/Slicer.h"
#include "core/SytrusEngine.h"
#include "core/Templates.h"
#include "core/Vocodex.h"
#include "core/TimeStretch.h"
#include "core/WorkerPool.h"
#include "core/RtStats.h"
#include "core/FixedBlock.h"
#include "core/ParametricEqLinear.h"
#include "core/Song.h"
#include "core/NoteEdit.h"
#include "core/Recorder.h"
#include "core/KeyMap.h"
#include "core/SortOnce.h"
#include "core/RackOps.h"
#include "core/ChannelLoops.h"
#include "core/ChannelBurn.h"
#include "core/AutoSampler.h"
#include "core/ValueMath.h"
#include "core/Workspace.h"
#include "core/StartupSound.h"
#include "core/ExportOptions.h"
#include "core/Mp3Encoder.h"
#include "core/PluginMonitor.h"
#include "core/PluginSubstitutes.h"
#include "core/ClipOps.h"
#include "core/PlaylistOps.h"
#include "core/TouchKeyboard.h"
#include "core/HeldNotes.h"
#include "core/Templates.h"
#include "flp/Flp.h"
#include "ui/I18nData.h"
#include "ui/I18nLocale.h"
#include "core/CrashReport.h"

namespace
{
int failures = 0;
int checks = 0;
std::vector<std::pair<const char*, std::function<void()>>>& registry()
{
    static std::vector<std::pair<const char*, std::function<void()>>> r;
    return r;
}

struct Register
{
    Register (const char* name, std::function<void()> fn) { registry().emplace_back (name, std::move (fn)); }
};

#define TEST(name)                                   \
    static void name();                              \
    static Register reg_##name (#name, name);        \
    static void name()

#define CHECK(cond)                                                                      \
    do                                                                                   \
    {                                                                                    \
        ++checks;                                                                        \
        if (! (cond))                                                                    \
        {                                                                                \
            ++failures;                                                                  \
            std::printf ("  FAILED %s:%d: %s\n", __FILE__, __LINE__, #cond);             \
        }                                                                                \
    } while (false)

// Engines that compute in x87 80-bit long double (FL's own arithmetic) match their goldens to the last bit on x86. On
// ARM64 (Apple Silicon) long double is not the x87 format, so there they hold to a looser bound (round 117).
// LYNX_X87_TOL overrides it (measuring).
inline float armTolerance (float x86, float arm)
{
#if defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86)
    (void) arm;
    return x86;
#else
    if (const char* t = std::getenv ("LYNX_X87_TOL"))
        return (float) std::atof (t);
    return arm;
#endif
}
inline float x87Tolerance() { return armTolerance (2.0e-7f, 2.0e-4f); }
inline float granulizerTolerance()
{
#if defined(__APPLE__)
    return 2.0e-4f;
#else
    return armTolerance (1.0e-6f, 1.0e-5f);
#endif
}

// Engines whose goldens depend on the C library's sin/cos/exp/pow: Windows (mingw), glibc and musl hold the 1e-7
// goldens, Apple's libm does not (round 117), so on macOS the per-sample bound is looser.
inline float libmTolerance (float strict)
{
#if defined(__APPLE__)
    (void) strict;
    return 2.0e-4f;
#else
    return strict;
#endif
}

// One golden sample: relative bound (with a 1e-3 floor on macOS, where near-zero samples carry libm noise); prints the
// measured error when it fails.
inline bool goldenSampleClose (float y, float golden, float tol)
{
#if defined(__APPLE__)
    const float scale = std::max (1.0e-3f, std::abs (golden));
#else
    const float scale = std::max (1.0e-16f, std::abs (golden));
#endif
    if (std::abs (y - golden) <= tol * scale)
        return true;
    std::fprintf (stderr, "  golden sample %.9g vs %.9g (relative error %.3g, bound %.3g)\n", (double) y, (double) golden,
                  (double) (std::abs (y - golden) / scale), (double) tol);
    return false;
}

std::string realFlpDir;

// --- helpers for building synthetic .flp files -----------------------------

struct FlpBuilder
{
    std::vector<std::uint8_t> bytes;

    FlpBuilder (std::uint16_t ppq = 96)
    {
        put ("FLhd");
        u32 (6);
        u16 (0);
        u16 (1);
        u16 (ppq);
        put ("FLdt");
        u32 (0); // size is not used by the reader
    }

    void put (const char* s) { bytes.insert (bytes.end(), s, s + std::strlen (s)); }
    void u8 (std::uint8_t v) { bytes.push_back (v); }
    void u16 (std::uint16_t v) { u8 ((std::uint8_t) v); u8 ((std::uint8_t) (v >> 8)); }
    void u32 (std::uint32_t v) { u16 ((std::uint16_t) v); u16 ((std::uint16_t) (v >> 16)); }

    void varint (std::size_t n)
    {
        do
        {
            auto b = (std::uint8_t) (n & 0x7F);
            n >>= 7;
            u8 (n ? (std::uint8_t) (b | 0x80) : b);
        } while (n);
    }

    void data (std::uint8_t id, const std::vector<std::uint8_t>& payload)
    {
        u8 (id);
        varint (payload.size());
        bytes.insert (bytes.end(), payload.begin(), payload.end());
    }

    void ascii (std::uint8_t id, const std::string& s)
    {
        std::vector<std::uint8_t> p (s.begin(), s.end());
        p.push_back (0);
        data (id, p);
    }

    void utf16 (std::uint8_t id, const std::u16string& s)
    {
        std::vector<std::uint8_t> p;
        for (auto c : s)
        {
            p.push_back ((std::uint8_t) c);
            p.push_back ((std::uint8_t) (c >> 8));
        }
        p.push_back (0);
        p.push_back (0);
        data (id, p);
    }
};

std::vector<std::uint8_t> noteRecord (std::uint32_t pos, std::uint16_t channel, std::uint32_t len, std::uint16_t key,
                                      std::uint8_t velocity)
{
    std::vector<std::uint8_t> r (24, 0);
    std::memcpy (&r[0], &pos, 4);
    std::memcpy (&r[6], &channel, 2);
    std::memcpy (&r[8], &len, 4);
    std::memcpy (&r[12], &key, 2);
    r[16] = 120;
    r[20] = 64;
    r[21] = velocity;
    return r;
}
} // namespace

// ---------------------------------------------------------------------------
// FLP

#define REA167K_REQUIRE(x) do { if (! (x)) { CHECK (x); return; } } while (false)
#include "Proll168gTests.inc"
int main (int argc, char** argv)
{
    if (argc > 1)
        realFlpDir = argv[1];

    const char* only = std::getenv ("LYNX_TEST"); // run only the tests whose name contains this
    for (auto& [name, fn] : registry())
    {
        if (only != nullptr && std::strstr (name, only) == nullptr)
            continue;
        std::printf ("%s\n", name);
        fn();
    }

    std::printf ("\n%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
