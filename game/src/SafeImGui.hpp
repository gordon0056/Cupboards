#pragma once

#include <imgui.h>
#include <iostream>

namespace game
{

class SafeImGui
{
public:
    static bool hasContext();
    static bool assertContext(const char* functionName);

    static bool Begin(const char* name, bool* p_open = nullptr,
                      ImGuiWindowFlags flags = 0);
    static void End();
    static void Text(const char* fmt, ...);
    static void TextUnformatted(const char* str);
    static bool Button(const char* label, const ImVec2& size = ImVec2(0, 0));
    static void SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0,
                                 const ImVec2& pivot = ImVec2(0, 0));
    static void SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0);
    static void Separator();
    static void SetCursorPosX(float x);
    static void SetCursorPosY(float y);
    static float CalcTextSize(const char* text, const char* text_end = nullptr,
                              bool hide_text_after_double_hash = false,
                              float wrap_width = -1.0f);
    static bool IsItemHovered(ImGuiHoveredFlags flags = 0);
    static void BeginTooltip();
    static void EndTooltip();
    static void PushStyleColor(ImGuiCol idx, ImU32 col);
    static void PopStyleColor(int count = 1);
};

}
