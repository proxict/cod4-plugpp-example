#ifndef PLUGPP_EXAMPLE_EXAMPLEPLUGIN_H
#define PLUGPP_EXAMPLE_EXAMPLEPLUGIN_H

#include <cod4-plugpp/Plugin.hpp>
#include <cod4-plugpp/utils/clientUtils.hpp>
#include <cod4-plugpp/utils/netUtils.hpp>
#include <cod4-plugpp/utils/stringUtils.hpp>

#include "plugpp-example/io/print.h"

template <typename T>
struct fmt::formatter<plugpp::Optional<T>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const plugpp::Optional<T>& s, FormatContext& ctx) {
        if (s) {
            return format_to(ctx.out(), "{}", *s);
        }
        return format_to(ctx.out(), "null");
    }
};

namespace example {

class ExamplePlugin final : public plugpp::Plugin {
public:
    /// Constructor gets called when the plugin is loaded
    explicit ExamplePlugin(const std::string& greet) { io::println("^2{}", greet); }

    /// Destructor gets called when the plugin is unloaded or when the server quits
    virtual ~ExamplePlugin() noexcept = default;

    /// Gets called whenever a client sends a chat-message
    ///
    /// @param message The message the client sent.
    /// @param slot Slot number of the player sending the message.
    /// @param mode Determines if the message is public (mode = 0) or team-only (mode = 1).
    /// @returns MessageVisibility::SHOW to allow the message to be sent or MessageVisibility::HIDE to prevent
    /// the message from sending.
    virtual plugpp::MessageVisibility
    onMessageSent(const std::string& message, int slot, int mode) final override {
        auto client = plugpp::getClientBySlot(slot);
        if (!client) {
            return plugpp::MessageVisibility::SHOW;
        }
        client_t* cl = *client;
        io::println("Player {} sent a {} chat-message: {}",
                    plugpp::removeColor(cl->name),
                    mode == 1 ? "team" : "public",
                    message);

        const bool isSwearWord = message.find("swearword") == std::string::npos;
        return isSwearWord ? plugpp::MessageVisibility::SHOW : plugpp::MessageVisibility::HIDE;
    }

    virtual plugpp::PluginInfo onPluginInfoRequest() final override {
        return plugpp::PluginInfo(1,
                                  0,
                                  "Example plugin",
                                  "Example of cod4-plugpp",
                                  "This example shows basic usage of the plugin's callback functions");
    }

    /// Gets called whenever a client wants to connects to the server
    ///
    /// This callback is called before the client authenticates with a player or a steam ID, but decision to
    /// allow or reject the player can still be made based on the provided information. @see
    /// @ref Plugin::onPlayerGotAuthInfo() or @ref Plugin::onPlayerGetBanStatus() functions if you want to
    /// grant or reject the access based on the player's steam or player ID.
    ///
    /// @param slot Slot number of the connecting player.
    /// @param netaddr IP address of the connecting player.
    /// @param userinfo The userinfo string the client sends during connecting. It's a backslash separated
    /// key-value list.
    /// @returns @ref plugpp::NoKick to allow the player to join or @ref plugpp::Kick to kick the player with
    /// the given reason.
    virtual plugpp::Kick onPlayerConnect(int slot, netadr_t* netaddr, const char* userinfo) final override {
        io::println("Client in slot {} joining the server from IP address {} with userinfo: {}",
                    slot,
                    plugpp::toStr(netaddr),
                    userinfo);
        return slot == 20 ? plugpp::Kick("Only ^1very special ^7players can join this slot") : plugpp::NoKick;
    }

    /// Gets called whenever a client disconnects from the server
    ///
    /// @param client The player disconnecting.
    /// @param reason Reason for the disconnect (can be a kick message, EXE_DISCONNECT or a violation
    /// message).
    virtual void onPlayerDisconnect(client_t* client, const char* reason) final override {
        io::println("Client {} just left the server. Reason: {}", plugpp::removeColor(client->name), reason);
    }

    /// Gets called when a player gets banned (can be a result of e.g., calling the banUser/banClient command)
    ///
    /// @param banInfo A structure containing information about the ban.
    virtual void onPlayerAddBan(baninfo_t* banInfo) final override {
        io::println("Player {} with player ID {} got {} banned for '{}'. Banned by admin {} with steam ID {}",
                    plugpp::removeColor(banInfo->playername),
                    banInfo->playerid,
                    banInfo->duration == 0 ? "permanently" : "temporarily",
                    banInfo->message,
                    plugpp::removeColor(banInfo->adminname),
                    banInfo->adminsteamid);
    }

    /// Gets called when a player gets unbanned (can be a result of e.g., calling the unbanUser/unbanClient
    /// command)
    ///
    /// @param banInfo A structure containing information about the ban.
    virtual void onPlayerRemoveBan(baninfo_t* banInfo) final override {
        io::println("Player {} with player ID {} got unbanned by admin {} with steam ID {}",
                    plugpp::removeColor(banInfo->playername),
                    banInfo->playerid,
                    plugpp::removeColor(banInfo->adminname),
                    banInfo->adminsteamid);
    }

    /// Gets called whenever a client wants to connects to the server
    ///
    /// This callback is called after the @ref Plugin::onPlayerConnect() function.
    ///
    /// @param cl The client connecting to the server.
    /// @param from The IP address the client is connecting from..
    /// @param playerid The player's player ID.
    /// @param steamid The player's steam ID.
    /// @param returnNow If false (default), the client proceeds with connecting unless they get kicked by
    /// this function's return value. If set to true, the client will keep resending the authentication
    /// request, this this function will keep getting called. This is useful when this callback's
    /// implementation requires longer time to determine whether the client's connection request should be
    /// granted or rejected. For example, running a remote database request in a thread to check for a ban
    /// record and waiting for result in this callback.
    /// @returns @ref plugpp::NoKick to allow the player to join or @ref plugpp::Kick to kick the player with
    /// the given reason.
    virtual plugpp::Kick onPlayerGotAuthInfo(client_t* cl,
                                             netadr_t* from,
                                             uint64_t& playerid,
                                             uint64_t& steamid,
                                             bool& returnNow) final override {
        (void)returnNow;
        io::println("Client {} connecting from {} authenticated with player ID {} and steam ID {}",
                    plugpp::removeColor(cl->name),
                    plugpp::toStr(from),
                    playerid,
                    steamid);
        return plugpp::NoKick;
    }

    /// Gets called whenever a client wants to connects to the server
    ///
    /// This callback is called after the @ref Plugin::onPlayerGotAuthInfo() function.
    ///
    /// @param banInfo A structure containing information about the ban.
    /// @returns @ref plugpp::NoKick to allow the player to join or @ref plugpp::Kick to kick the player with
    /// the given reason.
    virtual plugpp::Kick onPlayerGetBanStatus(baninfo_t* banInfo) final override {
        io::println("Checking ban status for player {} with player ID {} and steam ID {}",
                    plugpp::removeColor(banInfo->playername),
                    banInfo->playerid,
                    banInfo->steamid);
        return plugpp::NoKick;
    }

    /// Gets called when the @ref plugpp::onPlayerGetBanStatus() function returns @ref plugpp::NoKick and the
    /// player is thus allowed to join the server
    ///
    /// This callback is called after the @ref Plugin::onPlayerGetBanStatus() function.
    ///
    /// @param client The client joining the server.
    virtual void onPlayerAccessGranted(client_t* client) final override {
        io::println("Player {} with player ID {} joined the server",
                    plugpp::removeColor(client->name),
                    client->playerid);
    }

    /// Gets called before level fast restart (map fast restart)
    virtual void onPreFastRestart() final override { io::println("The map is about to get reloaded"); }

    /// Gets called after level fast restart (map fast restart)
    virtual void onPostFastRestart() final override { io::println("Map reloaded"); }

    /// Gets called at the end of a level (end of map)
    virtual void onExitLevel() final override { io::println("Map ended"); }

    /// Gets called at the beginning of a level (beginning of a map)
    virtual void onSpawnServer() final override {
        char mapname[256];
        Plugin_Cvar_VariableStringBuffer("mapname", mapname, sizeof(mapname));
        io::println("Map {} loeded", mapname);
    }

    /// Gets called every server frame (sv_fps times per second, usually 20 Hz)
    virtual void onFrame() final override {
        static constexpr size_t ONE_SECOND = 20; // 20 Hz
        static std::size_t i = 0;
        if (++i % (10 * ONE_SECOND) == 0) {
            io::println("10 second timer");
        }
    }

    /// Gets called when a player spawns
    /// @param entity The entity corresponding to the player.
    virtual void onClientSpawn(gentity_t* entity) final override {
        if (entity && entity->client) {
            io::println("Client in slot {} spawned", entity->client->ps.clientNum);
        }
    }

    /// Gets called when the player enteres the world
    ///
    /// Entering the world usually means that the player gets presented with the mod's screen where they can
    /// join a team, select a class, etc.
    ///
    /// @param client The client who entered the world.
    /// @param firstTime true if this was the first time the player has entered the world over the period of
    /// the player being connected to the server, false otherwise.
    virtual void onClientEnteredWorld(client_t* client, bool firstTime) final override {
        if (firstTime) {
            Plugin_ChatPrintf(Plugin_GetClientNumForClient(client), "^2Welcome, %s^2!", client->name);
        }
    }

    /// Gets called when the player's userinfo changes
    ///
    /// Userinfo is a string the client sends during connecting and when it changes. It's a backslash
    /// separated key-value list.
    ///
    /// @param client The player whose userinfo has changed.
    virtual void onClientUserInfoChanged(client_t* client) final override {
        io::println(
            "Players userinfo has changed for {}: {}", plugpp::removeColor(client->name), client->userinfo);
    }

    /// Gets called whenever a client sends a move command
    /// @param client The client who sent the move command.
    /// @param ucmd The move command.
    virtual void onClientMoveCommand(client_t* client, usercmd_t* ucmd) final override {
        (void)client;
        (void)ucmd;
    }

    /// Gets called when a player gets killed
    /// @param self The entity of the player who died.
    /// @param inflictor The entity that caused the death (might be a grenade, car/barrel explosion, etc).
    /// @param attacker The entity of the player who initiated the death.
    /// @param damage The amount of damage inflicted.
    /// @param meansOfDeath An index to the defined array of means of death.
    /// @param iWeapon An index of the weapon that was used to kill the player. The weapon's name can be
    /// quieried using the BG_WeaponName() function.
    /// @param hitLocation An index to the defined array of hit locations.
    virtual void onPlayerKilled(gentity_s* self,
                                gentity_s* inflictor,
                                gentity_s* attacker,
                                int damage,
                                int meansOfDeath,
                                int iWeapon,
                                hitLocation_t hitLocation) final override {
        static constexpr std::string_view MEANS_OF_DEATH_NAMES[] = {
            "MOD_UNKNOWN",        "MOD_PISTOL_BULLET", "MOD_RIFLE_BULLET",      "MOD_GRENADE",
            "MOD_GRENADE_SPLASH", "MOD_PROJECTILE",    "MOD_PROJECTILE_SPLASH", "MOD_MELEE",
            "MOD_HEAD_SHOT",      "MOD_CRUSH",         "MOD_TELEFRAG",          "MOD_FALLING",
            "MOD_SUICIDE",        "MOD_TRIGGER_HURT",  "MOD_EXPLOSIVE",         "MOD_IMPACT",
        };

        static constexpr std::string_view HITLOC_NAMES[] = {
            "HITLOC_NONE",      "HITLOC_HELMET",    "HITLOC_HEAD",      "HITLOC_NECK",
            "HITLOC_TORSO_UPR", "HITLOC_TORSO_LWR", "HITLOC_R_ARM_UPR", "HITLOC_L_ARM_UPR",
            "HITLOC_R_ARM_LWR", "HITLOC_L_ARM_LWR", "HITLOC_R_HAND",    "HITLOC_L_HAND",
            "HITLOC_R_LEG_UPR", "HITLOC_L_LEG_UPR", "HITLOC_R_LEG_LWR", "HITLOC_L_LEG_LWR",
            "HITLOC_R_FOOT",    "HITLOC_L_FOOT",    "HITLOC_GUN",       "HITLOC_NUM",
        };

        static auto getClientNum = [](gentity_s* entity) -> plugpp::Optional<int> {
            if (entity && entity->client) {
                return entity->client->ps.clientNum;
            }
            return plugpp::NullOptional;
        };

        io::println("self {}; inflictor {}; attacker {}; damage {}; meansOfDeath {}; iWeapon {}; hitLoc {}",
                    getClientNum(self),
                    getClientNum(inflictor),
                    getClientNum(attacker),
                    damage,
                    MEANS_OF_DEATH_NAMES[meansOfDeath],
                    BG_WeaponName(iWeapon),
                    HITLOC_NAMES[hitLocation]);
    }

    /// Gets called when a player requests joining to a reserved (password protected) slot
    /// @param from The IP address where the player is trying to connect from.
    /// @returns plugpp::ReservedSlotRequest::ALLOW to allow the player to join or
    /// plugpp::ReservedSlotRequest::DENY to deny the request.
    virtual plugpp::ReservedSlotRequest onPlayerReservedSlotRequest(netadr_t* from) final override {
        return from->type == netadrtype_t::NA_IP && from->ip[0] == 12 && from->ip[1] == 34 &&
                       from->ip[2] == 56 && from->ip[3] == 78
                   ? plugpp::ReservedSlotRequest::ALLOW
                   : plugpp::ReservedSlotRequest::DENY;
    }

    /// Gets called when a `usercall` function gets called from a GSC script.
    ///
    /// Function arguments passed from the GSC script can be acquired using the @ref Plugin_Scr_GetNumParam(),
    /// @ref Plugin_Scr_GetInt(), @ref Plugin_Scr_GetFloat(), @ref Plugin_Scr_GetString(),
    /// @ref Plugin_Scr_GetEntity(), @ref Plugin_Scr_GetConstString(), @ref Plugin_Scr_GetType(),
    /// @ref Plugin_Scr_GetVector(), @ref Plugin_Scr_GetFunc() functions.
    /// As opposed to the @ref onScrUsercallMethod(), this function is called without a player object.
    ///
    /// @param functionName The function name supplied to the `usercall` GSC function call.
    virtual void onScrUsercallFunction(const std::string& functionName) final override {
        io::println("A usercall '{}' was issued from a GSC script", functionName);
    }

    /// Gets called when a `usercall` method gets called from a GSC script.
    ///
    /// Metrod arguments passed from the GSC script can be acquired using the @ref Plugin_Scr_GetNumParam(),
    /// @ref Plugin_Scr_GetInt(), @ref Plugin_Scr_GetFloat(), @ref Plugin_Scr_GetString(),
    /// @ref Plugin_Scr_GetEntity(), @ref Plugin_Scr_GetConstString(), @ref Plugin_Scr_GetType(),
    /// @ref Plugin_Scr_GetVector(), @ref Plugin_Scr_GetFunc() functions.
    /// As opposed to the @ref onScrUsercallFunction(), this function is called with a specific player object.
    ///
    /// @param functionName The function name supplied to the `usercall` GSC method call.
    /// @param slot The slot of the player this function was called for.
    virtual void onScrUsercallMethod(const std::string& methodName, int slot) final override {
        auto client = plugpp::getClientBySlot(slot);
        if (!client) {
            return;
        }
        client_t* cl = *client;
        io::println("A usercall '{}' was issued for player {} from a GSC script",
                    methodName,
                    plugpp::removeColor(cl->name));
    }

    /// This function is currently never called
    virtual void onModuleLoaded(client_t* client, const std::string& fullpath, long checksum) final override {
        (void)client;
        (void)fullpath;
        (void)checksum;
    }

    /// Gets called when a screenshot arrives on the server
    /// @param client The client captured in the arrived screenshot.
    /// @param path The path to the screenshot.
    virtual void onScreenshotArrived(client_t* client, const std::string& path) final override {
        io::println("Screenshot '{}' arrived for player {}", path, plugpp::removeColor(client->name));
    }

    /// Gets called whenever there is a UDP packet event on the network
    ///
    /// This function is called only when compiled with -DOVERRIDE_UDP_EVENT_CALLBACKS
    ///
    /// @param from The IP address of the sender.
    /// @param data The pointer to the received data.
    /// @param size The size of the received data.
    /// @returns true if the packet should be dropped, false otherwise.
    virtual bool onUdpNetEvent(netadr_t* from, void* data, int size) final override {
        (void)from;
        (void)data;
        (void)size;
        return false;
    }

    /// Gets called whenever a UDP packet is sent from the server
    ///
    /// This function is called only when compiled with -DOVERRIDE_UDP_EVENT_CALLBACKS
    ///
    /// @param to The IP address of the receiver.
    /// @param data The pointer to the transmitted data.
    /// @param len The size of the transmitted data.
    /// @returns true if the packet should be dropped, false otherwise.
    virtual bool onUdpSend(netadr_t* to, void* data, int len) final override {
        (void)to;
        (void)data;
        (void)len;
        return false;
    }
};

} // namespace example

#endif // PLUGPP_EXAMPLE_EXAMPLEPLUGIN_H
