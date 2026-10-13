// The titanlink built-in's menu tab: Titanfall (the link, pilot mode, your Titan). Registered through
// sco.ui by titanlink_plugin.cpp and drawn by the menu shell; every button runs the same work as the
// built-in's commands.
#include "titanlink.h"
#include "../tabs.h"
#include "../../menu_ui.h"
#include "../../third_party/imgui/imgui.h"
#include <cstdio>

void DrawTitanLinkTab(void*, void*) {
    static char reply[200] = "";
    const TlStatus& s = TlGetStatus();
    SectionHeading("Titanfall 2");
    if (!s.linkOpen)        ImGui::TextUnformatted("连接：已关闭");
    else if (!s.linked)     ImGui::TextUnformatted(s.launched ? "连接：已开启，等待《泰坦陨落 2》启动" : "连接：已开启，等待《泰坦陨落 2》");
    else if (!s.inMatch)    ImGui::Text("连接：已连接《泰坦陨落 2》（%u 毫秒），正在加载对局", s.peerAgeMs);
    else                    ImGui::Text("连接：《泰坦陨落 2》对局中（%u 毫秒）", s.peerAgeMs);
    if (s.frameStatus[0]) ImGui::Text("画面：%s", s.frameStatus);
    if (s.peerLog[0]) ImGui::Text("泰坦陨落消息：%s", s.peerLog);
    if (s.game[0]) ImGui::Text("游戏目录：%s（%s，%s）", s.game, s.mode, s.map);

    SectionHeading("铁驭模式");
    if (PrimaryButton(s.pilot ? "停止铁驭模式 (F9)" : s.wantPilot ? "取消 (F9)" : "启动铁驭模式 (F9)")) {
        TlTogglePilot(reply, sizeof(reply));
        if (!s.pilot && s.wantPilot) MenuClose();
    }
    const float half = Columns(2);
    if (ImGui::Button("呼叫泰坦 (V)", ImVec2(half, 0))) TlCallTitan(reply, sizeof(reply));
    ImGui::SameLine();
    if (ImGui::Button(s.inTitan ? "离开泰坦 (E)" : "进入泰坦 (E)", ImVec2(half, 0))) TlEmbark(reply, sizeof(reply));
    if (ImGui::Button("关闭连接", ImVec2(-1, 0))) TlUnlink(reply, sizeof(reply));
    if (reply[0]) ImGui::TextUnformatted(reply);

    if (s.inMatch) {
        SectionHeading("你的泰坦");
        if (s.inTitan) ImGui::Text("正在驾驶，生命值 %.0f%%", s.titanHealth * 100.0f);
        else if (s.titanParked && s.titanDistance >= 0) ImGui::Text("停在 %.0f 米外，生命值 %.0f%%", s.titanDistance, s.titanHealth * 100.0f);
        else ImGui::TextUnformatted("暂无泰坦，按 V 呼叫");
        if (s.weapon[0]) ImGui::Text("武器：%s，弹匣 %d", s.weapon, s.clip);
    }
    Hint("需要安装 Northstar 和 TitanLink 插件的《泰坦陨落 2》（见 docs/bridges.md）。游戏目录、地图和模式在"
         " data\\titanlink.txt 中设置。铁驭模式下，星际公民仍会接收键盘输入。");
}
