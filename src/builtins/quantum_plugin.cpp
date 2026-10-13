// quantum: quantum travel and the Travel tab as a built-in plugin.
//
// quantum.travel, quantum.bookmark, quantum.save_bookmark and quantum.scan queue the same requests
// as the Travel tab, gated on the "quantum" capability (teleport's entity system found, which every
// travel request needs). The built-in's tick subscription runs ProcessQuantum (the quantum boost
// input, sounds and effects) and ProcessTravel (places, bookmarks, the scan). The mechanics, the
// boost detours and the Gladius drive patch stay in quantum.cpp and travel.cpp; their readiness
// stays the quantum.drive and quantum.boost capabilities. The saved spots live in the built-in's
// storage (data/storage/quantum.db, travel.cpp), opened here.
#include "builtins.h"
#include "builtin_store.h"
#include "tabs.h"
#include "../quantum.h"
#include "../teleport.h"
#include "../travel.h"
#include <cstdio>

namespace {

bool g_ticking = false;   // the tick subscription owns ProcessQuantum and ProcessTravel

const sco_plugin_info kInfo = {
    sizeof(sco_plugin_info), SCO_API_MAJOR, SCO_API_MINOR, "quantum", SCO_VERSION, "sc-offline",
};

constexpr const char* kCap = "quantum";
constexpr int kMaxPlaces = 3000;      // travel.cpp's list sizes
constexpr int kMaxBookmarks = 400;
constexpr double kMinAltitude = 100.0, kMaxAltitude = 20000.0;   // the Travel tab's slider

TravelPlace    g_places[kMaxPlaces];   // game thread only (commands run there)
TravelBookmark g_marks[kMaxBookmarks];

sco_result Travel(const sco_arg* args, uint32_t, void*, char* reply, uint32_t size) {
    const char* name = args[0].v.s;
    const double altitude = args[1].v.f;
    if (!name || !*name) {
        snprintf(reply, size, "请输入“旅行”页面中的地点名称（来自 locations.txt 或扫描结果）");
        return SCO_BAD_ARG;
    }
    if (!(altitude >= kMinAltitude && altitude <= kMaxAltitude)) {
        snprintf(reply, size, "高度必须在 %.0f 到 %.0f 米之间", kMinAltitude, kMaxAltitude);
        return SCO_BAD_ARG;
    }
    const int n = Travel_GetPlaces(g_places, kMaxPlaces);
    for (int i = 0; i < n; ++i) {
        if (_stricmp(g_places[i].name, name) != 0) continue;
        Travel_RequestPlace(g_places[i], static_cast<float>(altitude));
        snprintf(reply, size, "正在前往 %s（%s），抵达位置将显示在状态栏", g_places[i].name, g_places[i].system);
        return SCO_OK;
    }
    snprintf(reply, size, "地点列表中没有“%s”（共 %d 个地点）", name, n);
    return SCO_FAILED;
}

sco_result Bookmark(const sco_arg* args, uint32_t, void*, char* reply, uint32_t size) {
    const char* name = args[0].v.s;
    if (!name || !*name) {
        snprintf(reply, size, "请输入收藏位置名称（见“旅行”页面）");
        return SCO_BAD_ARG;
    }
    const int n = Travel_GetBookmarks(g_marks, kMaxBookmarks);
    for (int i = 0; i < n; ++i) {
        if (_stricmp(g_marks[i].name, name) != 0) continue;
        Travel_RequestBookmark(i);
        snprintf(reply, size, "正在前往“%s”（%s）", g_marks[i].name, g_marks[i].system);
        return SCO_OK;
    }
    snprintf(reply, size, "没有名为“%s”的收藏位置", name);
    return SCO_FAILED;
}

sco_result SaveBookmark(const sco_arg* args, uint32_t, void*, char* reply, uint32_t size) {
    const char* name = args[0].v.s ? args[0].v.s : "";
    if (strlen(name) > 63) {
        snprintf(reply, size, "位置名称最多 63 个字符");
        return SCO_BAD_ARG;
    }
    if (Travel_GetBookmarks(g_marks, kMaxBookmarks) >= kMaxBookmarks) {
        snprintf(reply, size, "已收藏 %d 个位置，请先删除一个", kMaxBookmarks);
        return SCO_FAILED;
    }
    Travel_RequestSaveBookmark(name);
    snprintf(reply, size, "正在将当前位置保存为“%s”，完成后会显示在状态栏", *name ? name : "（区域名称）");
    return SCO_OK;
}

sco_result Scan(const sco_arg*, uint32_t, void*, char* reply, uint32_t size) {
    float progress = 0;
    if (Travel_Scanning(progress)) {
        snprintf(reply, size, "扫描已在进行中（%.0f%%）", progress * 100.0f);
        return SCO_FAILED;
    }
    Travel_RequestScan();
    snprintf(reply, size, "正在扫描游戏已加载的地点并写入 locations_found.txt");
    return SCO_OK;
}

void OnTick(const char*, const void* data, void*) {
    if (!data || !g_tp.ok) return;
    ProcessQuantum();
    ProcessTravel(*static_cast<const uint32_t*>(data));
}

const sco_plugin_info* QuantumQuery() { return &kInfo; }

sco_result QuantumLoad(const sco_api* api, sco_plugin* self) {
    const sco_arg_def travel[2] = {
        BuiltinArg("place", SCO_ARG_STRING, "A place's name, as the Travel tab lists it (Daymar)"),
        BuiltinArg("altitude", SCO_ARG_FLOAT, "Metres above the ground on arrival (100 to 20000)"),
    };
    const sco_arg_def mark[1] = { BuiltinArg("name", SCO_ARG_STRING, "The saved spot's name") };
    const sco_arg_def save[1] = { BuiltinArg("name", SCO_ARG_STRING, "A name for it; empty = the zone's name") };
    sco_result r = RegisterBuiltinCommand(api, self, kCap, "quantum.travel", "Travel to place",
        "Teleports you to a planet, moon, station or other place from the Travel tab", Travel, travel, 2);
    if (r == SCO_OK) r = RegisterBuiltinCommand(api, self, kCap, "quantum.bookmark", "Go to saved spot",
        "Teleports you to one of the Travel tab's saved spots", Bookmark, mark, 1);
    if (r == SCO_OK) r = RegisterBuiltinCommand(api, self, kCap, "quantum.save_bookmark", "Save this spot",
        "Saves where you are as a named spot in the Travel tab", SaveBookmark, save, 1);
    if (r == SCO_OK) r = RegisterBuiltinCommand(api, self, kCap, "quantum.scan", "Scan places",
        "Lists everything the game has loaded and writes it to locations_found.txt", Scan);
    if (r == SCO_OK) r = api->subscribe(self, "tick", OnTick, nullptr);
    if (r != SCO_OK) return r;   // the host releases what was registered
    g_quantumStore.Open(api, self);   // before the first tick, which loads the saved spots
    // Its page of the menu (and keys), through sco.ui; the menu shell draws it (tabs.h).
    RegisterBuiltinTab(api, self, "quantum.travel", "旅行", kTabTravel, DrawTravelTab);
    g_ticking = true;
    return SCO_OK;
}

// A crash leaves g_ticking set, so dllmain doesn't take the quantum and travel ticks back.
void QuantumUnload() {
    g_quantumStore.Close();
    g_ticking = false;
}

}  // namespace

bool QuantumBuiltinOwnsTick() { return g_ticking; }

const sco::plugins::Builtin kQuantumBuiltin = { "quantum", QuantumQuery, QuantumLoad, QuantumUnload };
