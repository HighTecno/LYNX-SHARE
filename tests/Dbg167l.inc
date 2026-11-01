// temporary debug (not committed)
TEST (dbg167l_dump)
{
    const char* path = std::getenv ("LYNX_DBG_FLP");
    if (path == nullptr)
        return;
    std::ifstream in (path, std::ios::binary);
    std::vector<std::uint8_t> bytes ((std::istreambuf_iterator<char> (in)), {});
    const auto p = flp::parseProject (bytes);
    for (const auto& w : p.warnings)
        std::printf ("warn %s\n", w.c_str());
    std::printf ("flp channels %d arrangements %d\n", (int) p.channels.size(), (int) p.arrangements.size());
    for (const auto& a : p.arrangements)
        for (const auto& it : a.items)
            std::printf ("item kind %d index %d pos %u len %u track %d so %f eo %f\n", (int) it.kind, it.index, it.position, it.length, it.track, it.startOffset, it.endOffset);
    const auto s = lynx::importFlp (p);
    std::printf ("song channels %d clips %d\n", (int) s.channels.size(), (int) s.playlist.size());
    for (const auto& c : s.playlist)
        std::printf ("clip kind %d ch %d start %lld len %lld\n", (int) c.kind, c.channel, (long long) c.start, (long long) c.length);
    for (std::size_t i = 0; i < s.channels.size(); ++i)
        std::printf ("ch %d %s inst %d sample %s\n", (int) i, s.channels[i].name.c_str(), (int) s.channels[i].instrument, s.channels[i].samplePath.c_str());
}
