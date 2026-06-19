#pragma once

#include "screens/IScreen.hpp"
#include "TweenManager.hpp"
#include "GraphRenderer.hpp"
#include "MoveCommand.hpp"
#include "AudioManager.hpp"
#include "SettingsScreen.hpp"
#include "Localization.hpp"
#include "core/GameState.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

namespace game
{

class GameScreen : public IScreen
{
public:
    GameScreen(const std::string& levelPath, const std::string& assetsBase = "",
               AppSettings* settings = nullptr);
    ~GameScreen() override;

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;
    void renderImGui() override;
    bool isDone() const override;

    core::GameState& gameState();
    TweenManager& tweenManager();
    AudioManager& getAudioManager() { return audio_; }
    void restoreMoveHistory(const std::string& savePath);

    std::function<void()> onWin;
    std::function<void()> onMenu;

private:
    void loadLevel(const std::string& levelPath);
    void handlePieceSelection(sf::Vector2f mousePos);
    void handlePieceMove(sf::Vector2f mousePos);
    std::vector<sf::Vector2f> buildWaypoints(int fromNodeId, int toNodeId) const;
    void syncSettings();
    sf::Vector2f computeFieldOffset() const;

    core::GameState gameState_;
    TweenManager tweenManager_;
    GraphRenderer renderer_;
    AudioManager audio_;
    std::vector<MoveCommand> moveHistory_;

    std::vector<int> highlightedNodes_;
    bool done_ = false;
    bool animating_ = false;
    bool debugOverlay_ = false;
    mutable float fps_ = 0.0f;

    std::string assetsBase_;
    std::string levelPath_;
    AppSettings* settingsPtr_ = nullptr;
    sf::Vector2f fieldOffset_{0.0f, 40.0f};
};

}
