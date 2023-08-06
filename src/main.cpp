#include "plugpp-example/ExamplePlugin.h"
#include "plugpp-example/io/print.h"
#include "plugpp-example/literals.h"

#include <cod4-plugpp/PluginEntry.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

using namespace example::memory_literals;

/// Sink logging messages into the server's console
///
/// The format of the logged message is `[<log severity>] <message>`
template <typename Mutex>
class ConsoleSink : public spdlog::sinks::base_sink<Mutex> {
public:
    explicit ConsoleSink() = default;

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        example::io::println("[{}] {}", example::io::getLogLevel(msg.level), msg.payload);
    }

    void flush_() override {}
};

/// This is the plugin's entry point - shall be defined.
///
/// This function is automatically called when the plugin is loaded.
/// The PluginEntry::registerPlugin() shall be called inside this function.
void pluginMain(plugpp::PluginEntry& entry) {
    static constexpr std::size_t MAX_SIZE = 50_MiB;
    static constexpr std::size_t MAX_FILES = 10;
    using RotatingFileSink = spdlog::sinks::rotating_file_sink_mt;

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::make_shared<ConsoleSink<std::mutex>>())->set_level(spdlog::level::info);
    sinks.emplace_back(std::make_shared<RotatingFileSink>("log/example.log", MAX_SIZE, MAX_FILES))
        ->set_level(spdlog::level::debug);

    auto logger = std::make_shared<spdlog::logger>(
        "example", std::make_move_iterator(std::begin(sinks)), std::make_move_iterator(std::end(sinks)));
    logger->set_level(spdlog::level::trace);

    spdlog::set_default_logger(std::move(logger));
    spdlog::flush_every(std::chrono::seconds(30));

    entry.registerPlugin<example::ExamplePlugin>("Hello, plugin!");
}

