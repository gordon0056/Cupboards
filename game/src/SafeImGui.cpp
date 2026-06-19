#include "SafeImGui.hpp"

namespace game
{

bool SafeImGui::hasContext()
{
    return ImGui::GetCurrentContext() != nullptr;
}

bool SafeImGui::assertContext(const char* functionName)
{
    if (!hasContext())
    {
        std::cerr << "[SafeImGui] WARNING: ImGui::" << functionName
                  << " called without valid context." << std::endl;
        return false;
    }
    return true;
}

bool SafeImGui::Begin(const char* name, bool* p_open, ImGuiWindowFlags flags)
{
    if (!assertContext("Begin")) return false;
    return ImGui::Begin(name, p_open, flags);
}

void SafeImGui::End()
{
    if (!assertContext("End")) return;
    ImGui::End();
}

void SafeImGui::Text(const char* fmt, ...)
{
    if (!assertContext("Text")) return;
    va_list args;
    va_start(args, fmt);
    ImGui::TextV(fmt, args);
    va_end(args);
}

void SafeImGui::TextUnformatted(const char* str)
{
    if (!assertContext("TextUnformatted")) return;
    ImGui::TextUnformatted(str);
}

bool SafeImGui::Button(const char* label, const ImVec2& size)
{
    if (!assertContext("Button")) return false;
    return ImGui::Button(label, size);
}

void SafeImGui::SetNextWindowPos(const ImVec2& pos, ImGuiCond cond,
                                  const ImVec2& pivot)
{
    if (!assertContext("SetNextWindowPos")) return;
    ImGui::SetNextWindowPos(pos, cond, pivot);
}

void SafeImGui::SetNextWindowSize(const ImVec2& size, ImGuiCond cond)
{
    if (!assertContext("SetNextWindowSize")) return;
    ImGui::SetNextWindowSize(size, cond);
}

void SafeImGui::Separator()
{
    if (!assertContext("Separator")) return;
    ImGui::Separator();
}

void SafeImGui::SetCursorPosX(float x)
{
    if (!assertContext("SetCursorPosX")) return;
    ImGui::SetCursorPosX(x);
}

void SafeImGui::SetCursorPosY(float y)
{
    if (!assertContext("SetCursorPosY")) return;
    ImGui::SetCursorPosY(y);
}

float SafeImGui::CalcTextSize(const char* text, const char* text_end,
                               bool hide_text_after_double_hash,
                               float wrap_width)
{
    if (!assertContext("CalcTextSize")) return 0.0f;
    return ImGui::CalcTextSize(text, text_end,
                                hide_text_after_double_hash, wrap_width).x;
}

bool SafeImGui::IsItemHovered(ImGuiHoveredFlags flags)
{
    if (!assertContext("IsItemHovered")) return false;
    return ImGui::IsItemHovered(flags);
}

void SafeImGui::BeginTooltip()
{
    if (!assertContext("BeginTooltip")) return;
    ImGui::BeginTooltip();
}

void SafeImGui::EndTooltip()
{
    if (!assertContext("EndTooltip")) return;
    ImGui::EndTooltip();
}

void SafeImGui::PushStyleColor(ImGuiCol idx, ImU32 col)
{
    if (!assertContext("PushStyleColor")) return;
    ImGui::PushStyleColor(idx, col);
}

void SafeImGui::PopStyleColor(int count)
{
    if (!assertContext("PopStyleColor")) return;
    ImGui::PopStyleColor(count);
}

}
