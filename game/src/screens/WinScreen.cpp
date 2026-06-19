#include "WinScreen.hpp"

namespace game
{

WinScreen::WinScreen(int language) : language_(language) {}

void WinScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    (void)event; (void)window;
}

void WinScreen::update(float dt) { (void)dt; }

void WinScreen::render(sf::RenderTarget& target) { (void)target; }

void WinScreen::renderImGui()
{
    auto _ = [this](const char* en, const char* ru) { return tr(en, ru, language_); };

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin(_("Victory", "Победа"), nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x /= 2.0f; center.y /= 2.0f;

    ImGui::SetCursorPosX(center.x - ImGui::CalcTextSize(
        _("You Win!", "Победа!")).x / 2.0f);
    ImGui::SetCursorPosY(center.y - 60.0f);
    ImGui::Text("%s", _("You Win!", "Победа!"));

    ImGui::SetCursorPosX(center.x - 40.0f);
    ImGui::SetCursorPosY(center.y);
    if (ImGui::Button(_("Menu", "Меню"), ImVec2(80, 40)))
    {
        if (onMenu) onMenu();
    }

    ImGui::End();
}

bool WinScreen::isDone() const { return done_; }

}
