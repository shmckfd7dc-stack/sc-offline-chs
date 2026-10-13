#pragma once
// Shared by the launcher and menu. Read through a Unicode Windows path so the
// bundled font also works when the package lives in a Chinese-named directory.
#include <windows.h>
#include <cstdio>
#include <cwchar>
#include <initializer_list>
#include "third_party/imgui/imgui.h"

namespace sc_offline_chs {
inline ImFont* LoadFontFile(const wchar_t* path, float size) {
    if (!path || !*path) return nullptr;
    FILE* file = nullptr;
    if (_wfopen_s(&file, path, L"rb") != 0 || !file) return nullptr;
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return nullptr; }
    const long bytes = ftell(file);
    if (bytes <= 0 || bytes > 32 * 1024 * 1024 || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return nullptr;
    }
    void* data = ImGui::MemAlloc(static_cast<size_t>(bytes));
    if (!data) { fclose(file); return nullptr; }
    const size_t read = fread(data, 1, static_cast<size_t>(bytes), file);
    fclose(file);
    if (read != static_cast<size_t>(bytes)) { ImGui::MemFree(data); return nullptr; }
    // ImGui 1.92 loads glyphs on demand; the atlas owns this allocation.
    return ImGui::GetIO().Fonts->AddFontFromMemoryTTF(data, static_cast<int>(bytes), size);
}

inline ImFont* LoadChineseFont(const wchar_t* bundledPath, float size) {
    if (ImFont* font = LoadFontFile(bundledPath, size)) return font;
    wchar_t windows[MAX_PATH] = L"";
    const UINT len = GetWindowsDirectoryW(windows, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) return nullptr;
    for (const wchar_t* name : { L"msyh.ttc", L"simhei.ttf", L"simsun.ttc" }) {
        wchar_t path[MAX_PATH + 64];
        swprintf_s(path, L"%ls\\Fonts\\%ls", windows, name);
        if (ImFont* font = LoadFontFile(path, size)) return font;
    }
    return nullptr;
}
} // namespace sc_offline_chs
