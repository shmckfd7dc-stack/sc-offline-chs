# Noto Sans SC 中文字体

来源：[Google Fonts / Noto Sans SC](https://github.com/google/fonts/tree/main/ofl/notosanssc)。
原始文件：`NotoSansSC[wght].ttf`，获取日期：2026-10-13。
使用 fontTools 将 `wght` 固定为 400，生成常规字重 TTF；未对子集裁剪，保留字体全部字符。
此字体按 SIL Open Font License 1.1 分发，完整许可证见 `OFL-NotoSansSC.txt`。

启动器及游戏内菜单优先加载此文件，若缺失则尝试 Windows 微软雅黑、黑体、宋体。
请随汉化程序一起保留 `data/fonts` 目录及许可证。ImGui 按需加载字形。

原始文件 SHA-256：`a3041811a78c361b1de50f953c805e0244951c21c5bd412f7232ef0d899af0da`

生成文件 SHA-256：`a38952f8971a3b5f7ba195dc96249bd637172f854bc01adb242392bdfb9c5ebf`

生成命令：

```sh
fonttools varLib.instancer 'NotoSansSC[wght].ttf' wght=400 -o NotoSansSC-Regular.ttf
```
