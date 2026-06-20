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

    auto getSystemFontDirs = []() -> std::vector<std::string>
    {
        std::vector<std::string> dirs;
#ifdef _WIN32
        dirs.push_back("C:/Windows/Fonts");
        wchar_t buf[MAX_PATH + 2] = {};
        if (GetWindowsDirectoryW(buf, MAX_PATH))
        {
            std::wstring wdir = buf;
            dirs.push_back((std::filesystem::path(wdir) / "Fonts").string());
        }
#elif defined(__APPLE__)
        dirs.push_back("/Library/Fonts");
        dirs.push_back("/System/Library/Fonts");
        if (auto home = std::getenv("HOME"))
            dirs.push_back(std::string(home) + "/Library/Fonts");
#else
        dirs.push_back("/usr/share/fonts");
        dirs.push_back("/usr/local/share/fonts");
        if (auto home = std::getenv("HOME"))
        {
            dirs.push_back(std::string(home) + "/.fonts");
            dirs.push_back(std::string(home) + "/.local/share/fonts");
        }
#endif
        return dirs;
    };

    auto collectFonts = [](const std::vector<std::string>& dirs,
                           std::vector<std::string>& out)
    {
        for (const auto& d : dirs)
        {
            if (!std::filesystem::exists(d)) continue;
            for (auto& entry : std::filesystem::recursive_directory_iterator(
                     d, std::filesystem::directory_options::skip_permission_denied))
            {
                auto ext = entry.path().extension().string();
                if (ext == ".ttf" || ext == ".otf" || ext == ".ttc")
                    out.push_back(entry.path().string());
            }
        }
    };

    std::vector<std::string> fontCandidates;
    fontCandidates.push_back(fontPath);
    collectFonts(getSystemFontDirs(), fontCandidates);

    for (const auto& fp : fontCandidates)
    {
        if (!std::filesystem::exists(fp)) continue;
        if (io.Fonts->AddFontFromFileTTF(fp.c_str(), 18.0f, nullptr,
                io.Fonts->GetGlyphRangesCyrillic()))
        {
            std::cout << "[Font] Loaded: " << fp << std::endl;
        }
    }

    if (io.Fonts->Fonts.empty())
    {
        io.Fonts->AddFontDefault();
        std::cerr << "[Font] No fonts loaded, using default" << std::endl;
    }

    (void)ImGui::SFML::UpdateFontTexture();

    for (int i = 0; i < (int)io.Fonts->Fonts.Size; i++)
    {
        const ImFont* f = io.Fonts->Fonts[i];
        bool hasCyr = f->FindGlyph((ImWchar)0x0424) != nullptr;
        if (hasCyr)
        {
            io.FontDefault = io.Fonts->Fonts[i];
            std::cout << "[Font] Default set to Font[" << i << "] (has Cyrillic)" << std::endl;
            break;
        }
    }
    if (!io.FontDefault)
    {
        io.FontDefault = io.Fonts->Fonts[0];
        std::cerr << "[Font] WARNING: No font has Cyrillic!" << std::endl;
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
