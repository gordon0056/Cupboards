#include "MainMenuScreen.hpp"

namespace game
{

MainMenuScreen::MainMenuScreen(const std::string& levelsDir, const std::string& assetsBase,
                               AppSettings* settings)
    : levelsDir_(levelsDir), assetsBase_(assetsBase), settings_(settings)
{
    syncSettings();
    audio_.loadMusic(assetsBase_ + "/sounds/music_menu.ogg");
    audio_.playMusic();
}

void MainMenuScreen::syncSettings()
{
    if (settings_)
    {
        audio_.setMasterVolume(settings_->masterVolume);
        audio_.setMusicVolume(settings_->musicVolume);
        audio_.setSfxVolume(settings_->sfxVolume);
    }
}

void MainMenuScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    (void)event; (void)window;
}

void MainMenuScreen::update(float dt)
{
    (void)dt;
    syncSettings();
}

void MainMenuScreen::render(sf::RenderTarget& target)
{
    bool dark = settings_ ? settings_->darkTheme : true;
    target.clear(dark ? sf::Color(30, 30, 30) : sf::Color(220, 220, 220));
}

void MainMenuScreen::renderImGui()
{
    int lang = settings_ ? settings_->language : 0;
    auto _ = [lang](const char* en, const char* ru) { return tr(en, ru, lang); };

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin(_("Main Menu", "Главное меню"), nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoScrollbar);

    ImVec2 display = ImGui::GetIO().DisplaySize;
    float centerX = display.x / 2.0f;
    float titleY = display.y * 0.25f;
    float buttonY = titleY + 50.0f;
    float spacing = 45.0f;

    ImGui::SetCursorPosY(titleY);
    ImGui::SetCursorPosX(centerX - ImGui::CalcTextSize("Cupboards").x / 2.0f);
    ImGui::Text("Cupboards");

    ImGui::SetCursorPosY(buttonY);
    ImGui::SetCursorPosX(centerX - 100.0f);
    if (ImGui::Button(_("New Game", "Новая игра"), ImVec2(200, 35)))
    {
        audio_.stopMusic();
        if (onNewGame) onNewGame();
    }

    ImGui::SetCursorPosY(buttonY + spacing);
    ImGui::SetCursorPosX(centerX - 100.0f);
    if (ImGui::Button(_("Continue", "Продолжить"), ImVec2(200, 35)))
    {
        audio_.stopMusic();
        if (onContinue) onContinue();
    }

    ImGui::SetCursorPosY(buttonY + spacing * 2);
    ImGui::SetCursorPosX(centerX - 100.0f);
    if (ImGui::Button(_("Settings", "Настройки"), ImVec2(200, 35)))
    {
        if (onSettings) onSettings();
    }

    ImGui::SetCursorPosY(buttonY + spacing * 3);
    ImGui::SetCursorPosX(centerX - 100.0f);
    if (ImGui::Button(_("?  Help", "?  Помощь"), ImVec2(200, 35)))
        showHelp_ = true;

    ImGui::SetCursorPosY(buttonY + spacing * 4);
    ImGui::SetCursorPosX(centerX - 100.0f);
    if (ImGui::Button(_("About", "О программе"), ImVec2(200, 35)))
        showAbout_ = true;

    ImGui::SetCursorPosY(display.y - 60.0f);
    ImGui::SetCursorPosX(centerX - 100.0f);
    if (ImGui::Button(_("Exit", "Выход"), ImVec2(200, 35)))
    {
        audio_.stopMusic();
        if (onExit) onExit();
    }

    ImGui::End();

    if (showHelp_) renderHelpModal();
    if (showAbout_) renderAboutModal();
}

void MainMenuScreen::renderHelpModal()
{
    int lang = settings_ ? settings_->language : 0;
    auto _ = [lang](const char* en, const char* ru) { return tr(en, ru, lang); };

    ImGui::SetNextWindowSize(ImVec2(500, 380), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(
        ImGui::GetIO().DisplaySize.x / 2.0f - 250.0f,
        ImGui::GetIO().DisplaySize.y / 2.0f - 190.0f), ImGuiCond_Always);

    if (ImGui::Begin(_("Help", "Помощь"), &showHelp_,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        ImGui::SeparatorText(_("Goal", "Цель"));
        ImGui::TextWrapped("%s",
            _("Move each colored piece to its matching target slot.\n"
              "A semi-transparent circle shows where each piece must go.",
              "Переместите каждую фишку на её целевое место.\n"
              "Полупрозрачный круг показывает, куда нужно поместить фишку."));

        ImGui::SeparatorText(_("Controls", "Управление"));
        ImGui::TextWrapped("%s",
            _("- Left-click a piece to select it\n"
              "- Left-click a highlighted node to move\n"
              "- Ctrl+Z to undo last move\n"
              "- F1 to toggle debug info",
              "- ЛКМ на фишку, чтобы выбрать\n"
              "- ЛКМ на подсвеченный узел, чтобы переместить\n"
              "- Ctrl+Z отменить последний ход\n"
              "- F1 показать отладочную информацию"));

        ImGui::SeparatorText(_("Features", "Возможности"));
        ImGui::TextWrapped("%s",
            _("- 9 puzzles with increasing difficulty\n"
              "- Pieces move step-by-step along edges\n"
              "- Background music and sound effects\n"
              "- Undo support (Ctrl+Z)",
              "- 9 головоломок с возрастающей сложностью\n"
              "- Фишки перемещаются шаг за шагом по рёбрам\n"
              "- Фоновая музыка и звуковые эффекты\n"
              "- Отмена ходов (Ctrl+Z)"));

        ImGui::Dummy(ImVec2(0, 10));
        float btnW = 100.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btnW) / 2.0f);
        if (ImGui::Button(_("Close", "Закрыть"), ImVec2(btnW, 30)))
            showHelp_ = false;
    }
    ImGui::End();
}

void MainMenuScreen::renderAboutModal()
{
    int lang = settings_ ? settings_->language : 0;
    auto _ = [lang](const char* en, const char* ru) { return tr(en, ru, lang); };

    ImGui::SetNextWindowSize(ImVec2(420, 280), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(
        ImGui::GetIO().DisplaySize.x / 2.0f - 210.0f,
        ImGui::GetIO().DisplaySize.y / 2.0f - 140.0f), ImGuiCond_Always);

    if (ImGui::Begin(_("About", "О программе"), &showAbout_,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        ImGui::SeparatorText("Cupboards");
        ImGui::TextWrapped("%s",
            _("A sliding puzzle game built with C++17, SFML 2.6 and Dear ImGui.",
              "Игра-головоломка, созданная с использованием C++17, SFML 2.6 и Dear ImGui."));
        ImGui::TextWrapped("%s",
            _("Developer: gordon0056 | First release: 2023 | Current: 2026",
              "Разработчик: gordon0056 | Первый выпуск: 2023 | Текущая: 2026"));

        ImGui::Dummy(ImVec2(0, 10));
        float btnW = 100.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btnW) / 2.0f);
        if (ImGui::Button(_("Close", "Закрыть"), ImVec2(btnW, 30)))
            showAbout_ = false;
    }
    ImGui::End();
}

bool MainMenuScreen::isDone() const { return done_; }

}
