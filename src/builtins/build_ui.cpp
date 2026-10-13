// The build built-in's menu tab: Build (objects by group, placing and the base). Moved from
// menu.cpp unchanged; registered through sco.ui by build_plugin.cpp and drawn by the menu shell.
#include "tabs.h"
#include "../menu.h"
#include "../menu_ui.h"
#include "../third_party/imgui/imgui.h"
#include <cstdio>
#include <cstring>

// Translate presentation only; category keys are also used for SQ42 filtering.
static const char* CategoryLabel(const char* key) {
    static const struct { const char* key; const char* label; } labels[] = {
        { "outposts", "前哨站" }, { "guards", "守卫" }, { "outpost parts", "前哨站部件" },
        { "bunkers", "地堡" }, { "habs", "居住舱" }, { "ruins", "废墟" },
        { "fob", "前线基地" }, { "cover", "掩体" }, { "market", "市场" },
        { "base", "基地" }, { "furniture", "家具" }, { "storage", "储物" },
        { "lights", "灯光" }, { "turrets", "炮塔" }, { "deco", "装饰" },
        { "props", "道具" }, { "displays", "显示设备" }, { "sq42", "42 中队" },
    };
    for (const auto& entry : labels) if (strcmp(key, entry.key) == 0) return entry.label;
    return key;
}

void DrawBuildTab(void*, void*) {
    static int  build = 0, buildTab = 0;
    static char buildFilter[64] = "";
    const int buildables = Menu_BuildCount();
    SectionHeading("物品");
    if (buildables < 0) { Hint("正在加载建造物品（请先进入游戏世界）……"); return; }
    if (buildables == 0) { Hint("未找到建造物品，请检查 data\\buildables.txt。"); return; }
    if (build >= buildables) build = 0;
    if (ImGui::BeginTabBar("##buildTabs", ImGuiTabBarFlags_FittingPolicyScroll)) {
        for (int c = 0; c < Menu_BuildCategoryCount(); ++c) {
            char tab[40];
            snprintf(tab, sizeof(tab), "%s##cat%d", CategoryLabel(Menu_BuildCategoryName(c)), c);
            if (tab[0] >= 'a' && tab[0] <= 'z') tab[0] -= 'a' - 'A';
            if (ImGui::BeginTabItem(tab)) { buildTab = c; ImGui::EndTabItem(); }
        }
        ImGui::EndTabBar();
    }
    SearchBox("##buildFilter", "搜索全部物品", buildFilter, sizeof(buildFilter));
    const bool searching = buildFilter[strspn(buildFilter, " _")] != 0;
    if (ImGui::BeginChild("##buildList", ImVec2(0, 260), ImGuiChildFlags_Borders)) {
        for (int i = 0; i < buildables; ++i) {
            const char* name = Menu_BuildName(i);
            if (searching ? !MatchesFilter(name, buildFilter) : Menu_BuildCategoryOf(i) != buildTab) continue;
            char label[160];
            PrettyBuildName(label, sizeof(label) - 16, name);
            if (searching) {
                char tagged[160];
                snprintf(tagged, sizeof(tagged), "%s   (%s)", label, CategoryLabel(Menu_BuildCategory(i)));
                strcpy_s(label, sizeof(label) - 16, tagged);
            }
            snprintf(label + strlen(label), 16, "##%d", i);
            if (ImGui::Selectable(label, i == build)) build = i;
            ImGui::SetItemTooltip("%s", name);
        }
    }
    ImGui::EndChild();

    SectionHeading("放置");
    char picked[128];
    PrettyBuildName(picked, sizeof(picked), Menu_BuildName(build));
    ImGui::Text("已选择：%s", picked);
    float reach = Menu_BuildReach();
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderFloat("##reach", &reach, 5.0f, 300.0f, "放置距离 %.0f 米");
    ImGui::SetItemTooltip("设置物品在前方的放置距离。物品会落在视线指向的地面，或指定距离下方的地面上。");
    Menu_SetBuild(build, reach);
    const bool building = Menu_BuildModeActive();
    if (PrimaryButton(building ? "停止建造 (F6)" : "开始建造 (F6)")) {
        Menu_ToggleBuildMode();
        if (!building) MenuClose();
    }
    const float half = Columns(2);
    if (ImGui::Button("撤销上一步", ImVec2(half, 0))) Menu_BuildUndo();
    ImGui::SameLine();
    char clearLabel[48];
    snprintf(clearLabel, sizeof(clearLabel), "清空基地 (%d)###clearBase", Menu_BuildPlacedCount());
    if (ImGui::Button(clearLabel, ImVec2(half, 0))) Menu_BuildClear();
    Hint("建造时：左键放置，R 旋转，[ 和 ] 调整距离，退格键撤销，F6 停止。");
}
