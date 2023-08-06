#ifndef PLUGPP_EXAMPLE_IO_PRINT_H
#define PLUGPP_EXAMPLE_IO_PRINT_H

#include <spdlog/fmt/chrono.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <cod4-plugpp/PluginApi.h>

namespace example::io {

template <typename... T>
inline void print(fmt::format_string<T...> fmt, T&&... args) {
    try {
        Plugin_Printf("%s", fmt::format(fmt, std::forward<T>(args)...).c_str());
    } catch (const std::exception& e) {
        spdlog::error("Printing failed: {}", e.what());
    }
}

template <typename... T>
inline void println(fmt::format_string<T...> fmt, T&&... args) {
    try {
        Plugin_Printf("%s\n", fmt::format(fmt, std::forward<T>(args)...).c_str());
    } catch (const std::exception& e) {
        spdlog::error("Printing failed: {}", e.what());
    }
}

inline const char* getLogLevel(const spdlog::level::level_enum level) {
    switch (level) {
    case spdlog::level::trace:
        return "trace"; // white
    case spdlog::level::debug:
        return "^5debug^7"; // cyan
    case spdlog::level::info:
        return "^2info^7"; // green
    case spdlog::level::warn:
        return "^3warning^7"; // yellow
    case spdlog::level::err:
        return "^1error^7"; // red
    case spdlog::level::critical:
        return "^1critical^7"; // red
    default:
        return "unknown";
    }
}

} // namespace example::io

#endif // PLUGPP_EXAMPLE_IO_PRINT_H
