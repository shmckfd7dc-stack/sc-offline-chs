# sc-offline 简体中文界面

适用于 [shmckfd7dc-stack/sc-offline-chs](https://github.com/shmckfd7dc-stack/sc-offline-chs)，基于版本 **0.7.7**、提交 **85774b784d4cc8eb5dda75148387318486971ef6** 制作。

## 汉化内容

- 启动器的主要界面、插件页、操作确认、帮助，以及启动、安装、卸载、更新和常见错误提示。
- 游戏内玩家、旅行、飞船、船员、建造、联机、《42 中队》和菜单设置页面；可选桥接页面的界面文字。
- 传送、飞船生成、船员和创造模式等操作的常见状态反馈；建造分类名称。
- 启动器配置文件的中文注释、插件显示名称和 Discord 设置说明。
- 启动器与游戏菜单使用随包提供的 Noto Sans SC 中文字体。MSVC 统一使用 UTF-8；终端输出也使用 UTF-8。

已翻译 900 余处字符串。游戏本体、剧情、任务、游戏内部飞船/物品/地点名称及第三方插件自身文案不在此汉化范围内。底层技术日志、SDK 返回的诊断以及部分可选桥接状态仍可能为英文。不会修改内部命令名、资源 ID、配置键或快捷键。

## 下载或编译

### 在 GitHub Actions 编译

1. 上传本次汉化修改后，打开仓库的 **Actions**。如果 fork 的工作流未启用，先点击启用按钮。
2. 左侧选择 **简体中文 Windows 编译**，点击 **Run workflow**，选择 `main` 并运行。
3. 等待 `portable` 任务成功，打开该次运行页面，在 **Artifacts** 下载 **sc-offline-chs-windows-x64**。
4. 解压整个产物。必须同时保留 `sc-offline.exe`、`dinput8.dll`、`sc-offline.ini` 以及完整的 `data` 目录，尤其是 `data/fonts`。
5. 双击 `sc-offline.exe`。游戏内菜单键为 **M**，具体功能和使用条件参见原版 [README](README.md) 与 [功能文档](docs/features.md)。

该工作流编译启动器、主模组、创造模式和 Discord 插件，并使用官方发布的 `sco-sdk-2.0.0.zip` 编译和检查外部 NPC、检查器插件，最后打包数据和字体。它只生成可下载产物，不自动创建 Release。

仓库原有 `build` 工作流仍然保留。它的 `dinput8-release` 产物只有主 DLL 和启动器；使用该产物时，需要自行配齐数据、字体及插件。因此推荐直接下载上面的完整汉化产物。

### 在 Windows 本地编译

安装 Visual Studio 2026 的“使用 C++ 的桌面开发”工作负载和 CMake，然后在开发者 PowerShell 中执行：

```powershell
git clone --recurse-submodules https://github.com/shmckfd7dc-stack/sc-offline-chs.git
cd sc-offline-chs
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Release --parallel
```

Visual Studio 2022 可使用 `-G "Visual Studio 17 2022" -A x64 -T v143`。已有克隆先运行 `git submodule update --init`。

主程序输出到 `build/bin/Release`。运行时将仓库中的 `data` 目录与 `launcher/sc-offline.ini` 放到程序旁边。创造模式和 Discord DLL 在 `build/data/plugins` 下；外部 NPC 和检查器插件另按 [SDK 编译说明](docs/sdk-workflows.md) 编译。完整打包可直接使用 Actions 工作流。

## 字体和更新

中文字体及其许可位于 `data/fonts`。字体缺失时会尝试 Windows 的微软雅黑、黑体和宋体；Wine 或未安装中文系统字体的 Windows 尤其需要保留随包字体。字体采用 SIL OFL 1.1，来源和生成方式见 [字体说明](data/fonts/README.md)。项目原有 GPL-3.0 许可及署名保留。

汉化启动器的更新源已指向此 fork 的 Releases，避免自动更新覆盖成上游英文版。此 fork 尚无符合条件的 Release 时，检查更新会提示未找到发布版本；Actions 产物本身不属于 Release。后续发布仍需遵守项目原有的版本号、标签、压缩包名称和 manifest 规则。

## 验证状态

汉化制作环境为 macOS，CMake 实测提示项目仅支持 MSVC，因此这里不提供未经编译验证的 EXE/DLL。

本次检查结果：

- `tools/check.sh`：47 个编译单元，2 条原有基线诊断，新增 0 条。
- 914 处词典翻译通过 printf 占位符顺序与 ImGui 隐藏标识检查。
- 字体在仓库自带 ImGui 中实测：631 个非 ASCII 字符，缺失字形 0 个，并成功生成绘制顶点。
- 发布清单、NPC 发行结构、ImGui 固定版本、SDK 头文件门禁的 45 个 Python 测试通过；实际 SDK 头文件门禁、禁止原始扫描检查和内置结果测试通过。
- 启动器有效配置值及插件内部标识保持一致；工作流 YAML、打包 Python 语法和 `git diff --check` 通过。

最终 Windows 链接、窗口实际布局和游戏内操作仍需在 Windows / 游戏环境验证。新增 Actions 工作流尚未在你的 GitHub 仓库运行。
