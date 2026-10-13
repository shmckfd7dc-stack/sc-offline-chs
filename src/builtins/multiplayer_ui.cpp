// The multiplayer built-in's menu tab: host, join or leave a session, and who's in it. Registered
// through sco.ui by multiplayer_plugin.cpp and drawn by the menu shell.
#include "multiplayer.h"
#include "tabs.h"
#include "../menu_ui.h"
#include "../third_party/imgui/imgui.h"
#include <cstdio>

namespace {

char g_message[192] = "";   // the last Host / Join answer

void Answer(const char* err, const char* ok) { snprintf(g_message, sizeof(g_message), "%s", err ? err : ok); }

void SessionForm(MpSettings& s) {
    SectionHeading("你");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##mpName", "其他玩家看到的昵称", s.name, sizeof(s.name));
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##mpPass", "联机口令（至少 8 个字符）", s.passphrase, sizeof(s.passphrase),
                             ImGuiInputTextFlags_Password);
    ImGui::Checkbox("在此电脑记住口令", &s.rememberPass);
    ImGui::SetItemTooltip("口令将以明文保存在 data\\storage\\multiplayer.db 中。关闭后每次需重新输入。");

    SectionHeading("创建联机房间");
    ImGui::SetNextItemWidth(Columns(2));
    ImGui::InputInt("UDP 端口##mpHostPort", &s.hostPort, 0);
    if (PrimaryButton("创建房间")) Answer(Mp_Host(), "房间已创建，请将局域网地址、端口和口令告知其他玩家。");

    SectionHeading("加入联机房间");
    ImGui::SetNextItemWidth(Columns(2));
    ImGui::InputTextWithHint("##mpAddr", "房主地址（例如 192.168.1.20）", s.joinAddress, sizeof(s.joinAddress));
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-1);
    ImGui::InputInt("##mpJoinPort", &s.joinPort, 0);
    if (PrimaryButton("加入")) Answer(Mp_Join(), "正在加入……");
}

void PeerList() {
    const int n = Mp_PeerCount();
    if (!ImGui::BeginTable("##mpPeers", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) return;
    ImGui::TableSetupColumn("玩家");
    ImGui::TableSetupColumn("显示形象");
    ImGui::TableSetupColumn("飞船");
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();
    for (int i = 0; i < n; ++i) {
        MpPeerView v;
        if (!Mp_Peer(i, v)) continue;
        ImGui::PushID(i);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", v.name);
        ImGui::SetItemTooltip("玩家 %llu，房间 ID %016llx", static_cast<unsigned long long>(v.peer),
                              static_cast<unsigned long long>(v.sessionId));
        ImGui::TableNextColumn();
        ImGui::TextWrapped("%s", v.avatar);
        ImGui::TableNextColumn();
        ImGui::TextWrapped("%s", v.ship[0] ? v.ship : "-");
        ImGui::TableNextColumn();
        if (!v.self && ImGui::SmallButton("传送至")) Answer(Mp_GoTo(v.peer), "传送完成。");
        ImGui::PopID();
    }
    ImGui::EndTable();
}

}  // namespace

void DrawMultiplayerTab(void*, void*) {
    if (!Mp_Enabled()) {
        Hint("联机功能不可用（sc-offline.ini 中 multiplayer = off，或缺少 sco.net；详见 mod.log）。");
        return;
    }
    MpSettings& s = Mp_Settings();
    SectionHeading("联机房间");
    ImGui::Text("%s", Mp_StateText());
    if (Mp_LastReason()[0]) ImGui::TextColored(kMuted, "上次联机结束原因：%s", Mp_LastReason());
    if (g_message[0]) ImGui::TextWrapped("%s", g_message);
    if (Mp_InSession()) {
        if (ImGui::Button("离开房间", ImVec2(-1, 0))) { Mp_Leave(); g_message[0] = 0; }
        PeerList();
        if (!Mp_GhostsReady()) Hint("其他玩家的映像已禁用：此游戏版本缺少 spawn.entities 1.2 或 teleport.spatial。");
    } else {
        SessionForm(s);
    }

    SectionHeading("其他玩家的映像");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##mpAvatar", "其他玩家看到的 NPC 类型", s.avatarClass, sizeof(s.avatarClass));
    ImGui::SetItemTooltip("填写 npcs.txt 中的人类角色类型。其他玩家的 npcs.txt 包含该类型时，会以此显示你；\n"
                          "否则使用对方选择的类型。");
    ImGui::Checkbox("显示其他玩家的飞船", &s.showShips);
    if (ImGui::Button("保存设置", ImVec2(-1, 0))) Mp_SaveSettings();
    Hint("仅支持局域网（VPN 需配置 sc-offline.ini 的 multiplayer_allow）。消息使用口令签名，但不"
         "加密。玩家映像仅用于显示，不同步物理、伤害、任务或物品栏。");
    if (Mp_AllowText()[0]) ImGui::TextColored(kMuted, "额外允许：%s", Mp_AllowText());
}
