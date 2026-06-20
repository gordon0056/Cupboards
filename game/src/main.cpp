#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "Application.hpp"
#include "screens/MainMenuScreen.hpp"
#include "screens/MenuScreen.hpp"
#include "screens/GameScreen.hpp"
#include "screens/WinScreen.hpp"
#include "screens/SettingsScreen.hpp"
#include "SaveManager.hpp"
#include "SafeImGui.hpp"
#include "AssetValidator.hpp"
#include "AudioManager.hpp"
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#endif

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function<void()> cleanup)
        : cleanup_(std::move(cleanup)) {}
    ~ScopeGuard()
    {
        if (cleanup_) cleanup_();
    }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
private:
    std::function<void()> cleanup_;
};

static std::string getExeDirectory()
{
#ifdef _WIN32
    char buffer[MAX_PATH + 2] = {0};
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len == 0 || len >= MAX_PATH)
        return std::filesystem::current_path().string();
    std::filesystem::path p(buffer);
    return p.parent_path().string();
#else
    return std::filesystem::current_path().string();
#endif
}

static std::string findAssetsBase()
{
    std::string exeDir = getExeDirectory();
    std::string exeAssets = (std::filesystem::path(exeDir) / "assets").string();
    if (std::filesystem::exists(exeAssets + "/levels"))
    {
        std::cout << "[Assets] Found (exe-relative): " << exeAssets << std::endl;
        return exeAssets;
    }
    std::string cwdAssets = std::filesystem::absolute("game/assets").string();
    if (std::filesystem::exists(cwdAssets + "/levels"))
    {
        std::cout << "[Assets] Found (CWD-relative): " << cwdAssets << std::endl;
        return cwdAssets;
    }
    std::cerr << "[Assets] WARNING: Could not find assets directory!" << std::endl;
    return exeAssets;
}

static void applyImGuiTheme(const game::AppSettings& settings)
{
    ImGuiStyle& style = ImGui::GetStyle();
    if (settings.darkTheme)
    {
        ImGui::StyleColorsDark();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    }
    else
    {
        ImGui::StyleColorsLight();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
        style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    }
}

int main()
{
#ifdef __linux__
    setenv("ALSOFT_DRIVERS", "pulse,alsa,null", 1);
    auto pulseSrv = std::getenv("PULSE_SERVER");
    std::cout << "[Audio] ALSOFT_DRIVERS=pulse,alsa,null"
              << "  PULSE_SERVER=" << (pulseSrv ? pulseSrv : "(not set)") << std::endl;
#endif

    std::string assetsBase = findAssetsBase();
    std::string levelsDir = assetsBase + "/levels";
    std::string savePath = assetsBase + "/save.json";
    std::string fontPath = assetsBase + "/fonts/Nunito/static/Nunito-Regular.ttf";
    std::string settingsPath = assetsBase + "/settings.json";

    auto& validator = game::AssetValidator::instance();
    validator.addAsset(levelsDir, "Levels directory", true);
    validator.addAsset(fontPath, "Nunito font", false);

    auto levels = game::AssetValidator::scanDirectory(levelsDir, ".json");
    if (levels.empty())
    {
        std::cerr << "[AssetValidator] ERROR: No level files found in "
                  << levelsDir << std::endl;
        return 1;
    }
    std::cout << "[AssetValidator] Found " << levels.size() << " level file(s)" << std::endl;
    validator.validate();

    sf::RenderWindow window(sf::VideoMode(800, 600), "Cupboards");
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window))
    {
        std::cerr << "Failed to initialize ImGui-SFML" << std::endl;
        return 1;
    }

    ScopeGuard imGuiShutdown([]()
    {
        if (game::SafeImGui::hasContext())
            ImGui::SFML::Shutdown();
    });

    ImGuiIO& io = ImGui::GetIO();

    auto tryFontsUntilCyrillic = [&](const std::vector<std::string>& paths)
    {
        ImWchar cyrTest[] = {0x0401, 0x0424, 0x0430, 0x044F, 0};
        for (const auto& fp : paths)
        {
            if (!std::filesystem::exists(fp)) continue;
            io.Fonts->Clear();
            ImFont* font = io.Fonts->AddFontFromFileTTF(fp.c_str(), 18.0f, nullptr,
                io.Fonts->GetGlyphRangesCyrillic());
            if (!font) continue;
            if (!ImGui::SFML::UpdateFontTexture())
            {
                std::cerr << "[Font] UpdateFontTexture failed for: " << fp << std::endl;
                continue;
            }
            bool hasCyr = true;
            for (ImWchar* cp = cyrTest; *cp; cp++)
            {
                if (!font->FindGlyphNoFallback(*cp)) { hasCyr = false; break; }
            }
            unsigned int totalGlyphs = 0;
            for (int i = 0; i < font->IndexLookup.Size; i++)
            {
                if (font->IndexLookup[i] != 0) totalGlyphs++;
            }
            std::cout << "[Font] " << fp << ": " << totalGlyphs << " glyphs"
                      << (hasCyr ? " (has Cyrillic)" : " (NO Cyrillic!)") << std::endl;
            if (hasCyr)
            {
                io.FontDefault = font;
                return true;
            }
        }
        return false;
    };

    std::vector<std::string> fontCandidates;

#ifdef _WIN32
    fontCandidates.push_back(fontPath);
    {
        wchar_t buf[MAX_PATH + 2] = {};
        std::wstring winDir;
        if (GetWindowsDirectoryW(buf, MAX_PATH)) winDir = buf;
        std::string wf = winDir.empty() ? "C:/Windows" : (std::filesystem::path(winDir).string());
        std::string fontDir = wf + "/Fonts";
        fontCandidates.push_back(fontDir + "/seguiemj.ttf");
        fontCandidates.push_back(fontDir + "/segoeui.ttf");
        fontCandidates.push_back(fontDir + "/tahoma.ttf");
        fontCandidates.push_back(fontDir + "/arial.ttf");
    }
#elif defined(__APPLE__)
    fontCandidates.push_back("/System/Library/Fonts/SFNS.ttf");
    fontCandidates.push_back("/System/Library/Fonts/Helvetica.ttf");
    fontCandidates.push_back("/Library/Fonts/Arial.ttf");
    fontCandidates.push_back(fontPath);
#else
    fontCandidates.push_back("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    fontCandidates.push_back("/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf");
    fontCandidates.push_back("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf");
    fontCandidates.push_back("/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
    fontCandidates.push_back("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    fontCandidates.push_back(fontPath);
#endif

    if (!tryFontsUntilCyrillic(fontCandidates))
    {
        io.Fonts->Clear();
        io.Fonts->AddFontDefault();
        (void)ImGui::SFML::UpdateFontTexture();
        io.FontDefault = io.Fonts->Fonts[0];
        std::cerr << "[Font] No Cyrillic font found, using default" << std::endl;
    }

    game::AppSettings appSettings;
    if (std::filesystem::exists(settingsPath))
    {
        try
        {
            std::ifstream f(settingsPath);
            nlohmann::json j;
            f >> j;
            appSettings.masterVolume = j.value("masterVolume", 80.0f);
            appSettings.musicVolume = j.value("musicVolume", 70.0f);
            appSettings.sfxVolume = j.value("sfxVolume", 90.0f);
            appSettings.darkTheme = j.value("darkTheme", true);
            appSettings.language = j.value("language", 0);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[Settings] Failed to load: " << e.what() << std::endl;
        }
    }
    applyImGuiTheme(appSettings);

    auto saveSettings = [settingsPath](const game::AppSettings& s)
    {
        try
        {
            nlohmann::json j;
            j["masterVolume"] = s.masterVolume;
            j["musicVolume"] = s.musicVolume;
            j["sfxVolume"] = s.sfxVolume;
            j["darkTheme"] = s.darkTheme;
            j["language"] = s.language;
            std::ofstream f(settingsPath);
            f << j.dump(2);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[Settings] Failed to save: " << e.what() << std::endl;
        }
    };

    game::Application app;

    auto makeGameScreen = [&app, assetsBase, &appSettings, savePath](
                              const std::string& path)
    {
        auto gs = std::make_unique<game::GameScreen>(path, assetsBase, &appSettings);
        if (path == savePath)
            gs->restoreMoveHistory(savePath);
        gs->onWin = [&app, &appSettings]()
        {
            auto ws = std::make_unique<game::WinScreen>(appSettings.language);
            ws->onMenu = [&app]()
            {
                app.popScreen();
                app.popScreen();
            };
            app.pushScreen(std::move(ws));
        };
        gs->onMenu = [&app]() { app.popScreen(); };
        return gs;
    };

    auto showLevelSelect = [&app, levelsDir, assetsBase, &appSettings, &makeGameScreen]()
    {
        auto menuScreen = std::make_unique<game::MenuScreen>(levelsDir, assetsBase, &appSettings);
        menuScreen->onLevelSelected = [&app, &makeGameScreen](const std::string& levelPath)
        {
            app.pushScreen(makeGameScreen(levelPath));
        };
        menuScreen->onBack = [&app]() { app.popScreen(); };
        app.pushScreen(std::move(menuScreen));
    };

    auto mainMenu = std::make_unique<game::MainMenuScreen>(levelsDir, assetsBase, &appSettings);
    game::MainMenuScreen* mainMenuPtr = mainMenu.get();
    mainMenu->onNewGame = [&app, &showLevelSelect]() { showLevelSelect(); };
    mainMenu->onContinue = [&app, savePath, &makeGameScreen]()
    {
        if (std::filesystem::exists(savePath))
        {
            try
            {
                std::string levelPath;
                nlohmann::json levelJson;
                if (game::SaveManager::load(savePath, levelPath, levelJson))
                    app.pushScreen(makeGameScreen(savePath));
            }
            catch (const std::exception&) {}
        }
    };
    mainMenu->onSettings = [&app, &appSettings, mainMenuPtr, &saveSettings]()
    {
        auto ss = std::make_unique<game::SettingsScreen>(appSettings);
        ss->onBack = [&app]() { app.popScreen(); };
        ss->onApply = [&app, &appSettings, mainMenuPtr, &saveSettings]()
        {
            applyImGuiTheme(appSettings);
            mainMenuPtr->syncSettings();
            saveSettings(appSettings);
        };
        ss->onReset = [&app, &appSettings, mainMenuPtr, &saveSettings]()
        {
            applyImGuiTheme(appSettings);
            mainMenuPtr->syncSettings();
            saveSettings(appSettings);
        };
        app.pushScreen(std::move(ss));
    };
    mainMenu->onExit = [&window]() { window.close(); };
    app.pushScreen(std::move(mainMenu));

    const float fixedDt = 1.0f / 60.0f;
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) window.close();
            app.handleEvent(event, window);
        }

        accumulator += clock.restart();
        if (accumulator > sf::seconds(0.5f))
            accumulator = sf::seconds(0.5f);

        while (accumulator >= sf::seconds(fixedDt))
        {
            app.update(fixedDt);
            accumulator -= sf::seconds(fixedDt);
        }

        ImGui::SFML::Update(window, sf::seconds(fixedDt));
        app.render(window);
        app.renderImGui();
        ImGui::SFML::Render(window);
        window.display();
    }

    return 0;
}
