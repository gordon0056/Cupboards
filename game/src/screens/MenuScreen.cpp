#include "MenuScreen.hpp"
#include "core/LevelParser.hpp"
#include <iostream>
#include <filesystem>

namespace game
{

MenuScreen::MenuScreen(const std::string& levelsDir, const std::string& assetsBase,
                       AppSettings* settings)
    : settings_(settings)
{
    scanLevels(levelsDir);
    if (settings_)
    {
        audio_.setMasterVolume(settings_->masterVolume);
        audio_.setMusicVolume(settings_->musicVolume);
        audio_.setSfxVolume(settings_->sfxVolume);
    }
    if (!assetsBase.empty())
        audio_.loadMusic(assetsBase + "/sounds/music_menu.ogg");
    audio_.playMusic();
}

void MenuScreen::syncSettings()
{
    if (settings_)
    {
        audio_.setMasterVolume(settings_->masterVolume);
        audio_.setMusicVolume(settings_->musicVolume);
        audio_.setSfxVolume(settings_->sfxVolume);
    }
}

void MenuScreen::scanLevels(const std::string& levelsDir)
{
    levels_.clear();
    if (!std::filesystem::exists(levelsDir)) return;

    for (const auto& entry : std::filesystem::directory_iterator(levelsDir))
    {
        if (entry.path().extension() != ".json") continue;

        std::string fullPath = entry.path().string();
        LevelEntry le;
        le.path = fullPath;
        le.name = entry.path().filename().string();

        try
        {
            core::LevelData data = core::LevelParser::load(fullPath);
            le.name = data.name;
            le.valid = true;
        }
        catch (const std::exception& e)
        {
            le.valid = false;
            le.errorMessage = e.what();
            std::cerr << "Level load error (" << le.name << "): " << e.what() << std::endl;
        }

        levels_.push_back(le);
    }
}

void MenuScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    (void)event; (void)window;
}

void MenuScreen::update(float dt)
{
    (void)dt;
    syncSettings();
}

void MenuScreen::render(sf::RenderTarget& target)
{
    bool dark = settings_ ? settings_->darkTheme : true;
    target.clear(dark ? sf::Color(30, 30, 30) : sf::Color(220, 220, 220));
}

void MenuScreen::renderImGui()
{
    int lang = settings_ ? settings_->language : 0;
    auto _ = [lang](const char* en, const char* ru) { return tr(en, ru, lang); };

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin(_("Levels", "Уровни"), nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImVec2 display = ImGui::GetIO().DisplaySize;
    float centerX = display.x / 2.0f;

    ImGui::SetCursorPosY(display.y * 0.1f);
    ImGui::SetCursorPosX(centerX - ImGui::CalcTextSize("Cupboards").x / 2.0f);
    ImGui::Text("Cupboards");

    ImGui::SetCursorPosX(centerX - ImGui::CalcTextSize(
        _("Select a Level:", "Выберите уровень:")).x / 2.0f);
    ImGui::Text("%s", _("Select a Level:", "Выберите уровень:"));

    float startY = display.y * 0.25f;
    float itemHeight = 30.0f;
    float spacing = 8.0f;
    float curY = startY;

    for (size_t i = 0; i < levels_.size(); ++i)
    {
        const auto& entry = levels_[i];

        if (entry.valid)
        {
            ImGui::SetCursorPos(ImVec2(centerX - 100.0f, curY));
            if (ImGui::Button(entry.name.c_str(), ImVec2(200, itemHeight)))
            {
                audio_.stopMusic();
                done_ = true;
                if (onLevelSelected) onLevelSelected(entry.path);
            }
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
            ImGui::SetCursorPos(ImVec2(
                centerX - ImGui::CalcTextSize(entry.name.c_str()).x / 2.0f, curY));
            ImGui::TextDisabled("%s", entry.name.c_str());
            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("%s", _("Invalid level", "Неверный уровень"));
                ImGui::EndTooltip();
            }
        }

        curY += itemHeight + spacing;
    }

    ImGui::SetCursorPosY(display.y - 80.0f);
    ImGui::SetCursorPosX(centerX - 60.0f);
    if (ImGui::Button(_("Back", "Назад"), ImVec2(120, 35)))
    {
        done_ = true;
        if (onBack) onBack();
    }

    ImGui::End();
}

bool MenuScreen::isDone() const { return done_; }

}
