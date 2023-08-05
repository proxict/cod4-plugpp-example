#include <cod4-plugpp/Plugin.hpp>
#include <cod4-plugpp/PluginEntry.hpp>
#include <spdlog/spdlog.h>

class ExamplePlugin final : public plugpp::Plugin {
public:
    explicit ExamplePlugin(const std::string& greet) { spdlog::info("{}", greet); }

    virtual ~ExamplePlugin() noexcept = default;
};

void pluginMain(plugpp::PluginEntry& entry) {
    entry.registerPlugin<ExamplePlugin>("Hello, plugin!");
}

