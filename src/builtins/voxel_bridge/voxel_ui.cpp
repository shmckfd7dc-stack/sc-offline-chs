// The voxel_bridge built-in's menu tab: Voxel (the link, the building area, the crates). Registered
// through sco.ui by voxel_plugin.cpp and drawn by the menu shell; every button runs the same work as
// the built-in's commands.
#include "voxel.h"
#include "../tabs.h"
#include "../../menu_ui.h"
#include "../../third_party/imgui/imgui.h"
#include <cstdio>

void DrawVoxelTab(void*, void*) {
    static char reply[200] = "";
    const VxStatus& s = VxGetStatus();
    SectionHeading("体素游戏");
    if (!s.linkOpen)     ImGui::TextUnformatted("连接：已关闭");
    else if (!s.linked)  ImGui::TextUnformatted("连接：已开启，等待体素游戏");
    else                 ImGui::Text("连接：体素游戏 %s（%u 毫秒）", s.inWorld ? "已进入世界" : "已连接，尚未加载世界", s.peerAgeMs);
    if (s.peerStatus[0]) ImGui::Text("体素游戏：%s", s.peerStatus);
    if (s.peerLog[0]) ImGui::Text("最新消息：%s", s.peerLog);
    if (PrimaryButton(s.linkOpen ? "关闭桥接 (Ctrl+F9)" : "开启桥接 (Ctrl+F9)")) VxToggle(reply, sizeof(reply));

    SectionHeading("建造区域");
    if (!s.anchored) ImGui::TextUnformatted("暂无区域，开启桥接后将在你的位置创建");
    else ImGui::Text("位于 %s%s；你所在的方块坐标为 %.0f %.0f %.0f", s.zone[0] ? s.zone : "你的区域", s.paused ? "（你在区域外）" : "",
                     s.feet[0], s.feet[1], s.feet[2]);
    if (ImGui::Button("将区域移到这里", ImVec2(-1, 0))) VxAnchor(reply, sizeof(reply));
    ImGui::Text("箱子：%zu（还有 %zu 个待生成）", s.crates, s.pending);
    if (s.dropped) ImGui::Text("超过箱子数量限制的 %u 个实体方块未显示", s.dropped);
    ImGui::Text("方块类型 %s，大小 %.2f 米", s.block, s.blockSize);
    if (reply[0]) ImGui::TextUnformatted(reply);
    Hint("需要安装 sc-offline 模组的体素游戏（见 docs/bridges.md）。箱子类型和大小在 data\\voxel_bridge.txt 中设置。");
}
