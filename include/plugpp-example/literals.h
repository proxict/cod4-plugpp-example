#ifndef PLUGPP_EXAMPLE_LITERALS_H
#define PLUGPP_EXAMPLE_LITERALS_H

namespace example::memory_literals {

constexpr auto operator""_KiB(const unsigned long long x) {
    return 1024ULL * x;
}

constexpr auto operator""_MiB(const unsigned long long x) {
    return 1024ULL * 1024ULL * x;
}

constexpr auto operator""_GiB(const unsigned long long x) {
    return 1024ULL * 1024ULL * 1024ULL * x;
}

} // namespace example::memory_literals

#endif // PLUGPP_EXAMPLE_LITERALS_H
