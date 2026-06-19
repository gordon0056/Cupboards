#include "SettingsScreen.hpp"

namespace game
{

SettingsScreen::SettingsScreen(AppSettings& settings)
    : settings_(settings), edit_(settings) {}

void SettingsScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    (void)event; (void)window;
}

void SettingsScreen::update(float dt) { (void)dt; }

void SettingsScreen::render(sf::RenderTarget& target)
{
    target.clear(edit_.darkTheme ? sf::Color(30, 30, 30) : sf::Color(220, 220, 220));
}

void SettingsScreen::renderImGui()
{
    int lang = edit_.language;
    auto _ = [lang](const char* en, const char* ru) { return tr(en, ru, lang); };

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin(_("Settings", "Настройки"), nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoScrollbar);

    ImVec2 display = ImGui::GetIO().DisplaySize;
    float centerX = display.x / 2.0f;
    float startY = display.y * 0.12f;
    float spacing = 50.0f;
    float labelX = centerX - 180.0f;
    float sliderX = centerX - 80.0f;

    ImGui::SetCursorPosY(startY);
    ImGui::SetCursorPosX(centerX - ImGui::CalcTextSize(_("Settings", "Настройки")).x / 2.0f);
    ImGui::Text("%s", _("Settings", "Настройки"));

    float row = startY + spacing;
    ImGui::SetCursorPosY(row);
    ImGui::SetCursorPosX(labelX);
    ImGui::Text("%s", _("Master Volume", "Громкость"));
    ImGui::SetCursorPosX(sliderX);
    ImGui::PushItemWidth(200.0f);
    ImGui::SliderFloat("##master", &edit_.masterVolume, 0.0f, 100.0f, "%.0f%%");
    ImGui::PopItemWidth();

    row += spacing;
    ImGui::SetCursorPosY(row);
    ImGui::SetCursorPosX(labelX);
    ImGui::Text("%s", _("Music Volume", "Музыка"));
    ImGui::SetCursorPosX(sliderX);
    ImGui::PushItemWidth(200.0f);
    ImGui::SliderFloat("##music", &edit_.musicVolume, 0.0f, 100.0f, "%.0f%%");
    ImGui::PopItemWidth();

    row += spacing;
    ImGui::SetCursorPosY(row);
    ImGui::SetCursorPosX(labelX);
    ImGui::Text("%s", _("SFX Volume", "Эффекты"));
    ImGui::SetCursorPosX(sliderX);
    ImGui::PushItemWidth(200.0f);
    ImGui::SliderFloat("##sfx", &edit_.sfxVolume, 0.0f, 100.0f, "%.0f%%");
    ImGui::PopItemWidth();

    row += spacing;
    ImGui::SetCursorPosY(row);
    ImGui::SetCursorPosX(labelX);
    ImGui::Text("%s", _("Theme", "Тема"));
    ImGui::SetCursorPosX(sliderX);
    if (ImGui::RadioButton(_("Light", "Светлая"), !edit_.darkTheme))
        edit_.darkTheme = false;
    ImGui::SameLine();
    if (ImGui::RadioButton(_("Dark", "Тёмная"), edit_.darkTheme))
        edit_.darkTheme = true;

    row += spacing;
    ImGui::SetCursorPosY(row);
    ImGui::SetCursorPosX(labelX);
    ImGui::Text("%s", _("Language", "Язык"));
    ImGui::SetCursorPosX(sliderX);
    const char* langs[] = {"English", "Russian"};
    ImGui::PushItemWidth(200.0f);
    ImGui::Combo("##lang", &edit_.language, langs, IM_ARRAYSIZE(langs));
    ImGui::PopItemWidth();

    float btnY = display.y - 80.0f;
    ImGui::SetCursorPosY(btnY);
    ImGui::SetCursorPosX(centerX - 190.0f);
    if (ImGui::Button(_("Apply", "Применить"), ImVec2(120, 35)))
    {
        settings_ = edit_;
        if (onApply) onApply();
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(centerX - 60.0f);
    if (ImGui::Button(_("Reset", "Сброс"), ImVec2(120, 35)))
    {
        edit_ = AppSettings();
        settings_ = edit_;
        if (onReset) onReset();
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(centerX + 70.0f);
    if (ImGui::Button(_("Back", "Назад"), ImVec2(120, 35)))
    {
        if (onBack) onBack();
        done_ = true;
    }

    ImGui::End();
}

bool SettingsScreen::isDone() const { return done_; }

}
