#include "GameScreen.hpp"
#include "SaveManager.hpp"
#include "core/LevelParser.hpp"
#include "core/PathFinder.hpp"
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <filesystem>

namespace game
{

GameScreen::GameScreen(const std::string& levelPath, const std::string& assetsBase,
                       AppSettings* settings)
    : assetsBase_(assetsBase.empty() ? "game/assets" : assetsBase)
    , levelPath_(levelPath)
    , settingsPtr_(settings)
{
    loadLevel(levelPath);

    tweenManager_.onCompleteCallback = [this]()
    {
        animating_ = false;
        if (gameState_.isWinConditionMet())
        {
            audio_.stopMusic();
            audio_.playSound("win");
            std::error_code ec;
            std::filesystem::remove(assetsBase_ + "/save.json", ec);
            if (onWin) onWin();
        }
    };
}

GameScreen::~GameScreen()
{
    audio_.stopMusic();
    if (!gameState_.isWinConditionMet())
    {
        std::string savePath = assetsBase_ + "/save.json";
        SaveManager::save(savePath, levelPath_, gameState_, moveHistory_);
    }
}

void GameScreen::restoreMoveHistory(const std::string& savePath)
{
    try
    {
        std::ifstream file(savePath);
        if (!file.is_open()) return;
        nlohmann::json j;
        file >> j;
        if (!j.contains("move_history")) return;
        for (const auto& m : j["move_history"])
        {
            MoveCommand cmd;
            cmd.pieceId = m["pieceId"].get<int>();
            cmd.fromNodeId = m["fromNodeId"].get<int>();
            cmd.toNodeId = m["toNodeId"].get<int>();
            cmd.waypoints = buildWaypoints(cmd.toNodeId, cmd.fromNodeId);
            moveHistory_.push_back(cmd);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to restore move history: " << e.what() << std::endl;
    }
}

void GameScreen::loadLevel(const std::string& levelPath)
{
    core::LevelData level = core::LevelParser::load(levelPath);
    gameState_.initializeFromLevel(level);
    highlightedNodes_.clear();
    animating_ = false;

    fieldOffset_ = computeFieldOffset();

    if (!gameState_.level.bg_music.empty())
    {
        audio_.loadMusic(assetsBase_ + "/sounds/" + gameState_.level.bg_music);
        audio_.playMusic();
    }

    audio_.loadSound("click", assetsBase_ + "/sounds/click_piece.ogg");
    audio_.loadSound("move", assetsBase_ + "/sounds/click_move.ogg");
    audio_.loadSound("win", assetsBase_ + "/sounds/win.ogg");
}

sf::Vector2f GameScreen::computeFieldOffset() const
{
    if (gameState_.level.graph.nodes.empty())
        return sf::Vector2f(0.0f, 40.0f);

    float minX = FLT_MAX, maxX = FLT_MIN;
    float minY = FLT_MAX, maxY = FLT_MIN;

    for (const auto& node : gameState_.level.graph.nodes)
    {
        minX = std::min(minX, node.x);
        maxX = std::max(maxX, node.x);
        minY = std::min(minY, node.y);
        maxY = std::max(maxY, node.y);
    }

    float fieldW = maxX - minX + 60.0f;
    float fieldH = maxY - minY + 60.0f;
    float winW = 800.0f;
    float winH = 600.0f;
    float ox = (winW - fieldW) * 0.5f - minX + 30.0f;
    float oy = (winH - fieldH) * 0.5f - minY + 30.0f + 40.0f;

    return sf::Vector2f(ox, oy);
}

void GameScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::F1)
    {
        debugOverlay_ = !debugOverlay_;
        return;
    }

    if (animating_) return;

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Z &&
        (event.key.control || event.key.alt))
    {
        if (!moveHistory_.empty())
        {
            MoveCommand cmd = moveHistory_.back();
            moveHistory_.pop_back();

            cmd.undo(gameState_);

            auto waypoints = buildWaypoints(cmd.toNodeId, cmd.fromNodeId);

            if (!waypoints.empty())
            {
                tweenManager_.start(cmd.pieceId, waypoints);
                animating_ = true;
            }

            highlightedNodes_.clear();
            gameState_.selectedPieceId.reset();
        }
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        if (animating_) return;

        sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

        if (gameState_.selectedPieceId.has_value())
            handlePieceMove(mousePos);
        else
            handlePieceSelection(mousePos);
    }
}

void GameScreen::handlePieceSelection(sf::Vector2f mousePos)
{
    float hitRadius = 25.0f;

    for (const auto& piece : gameState_.pieces)
    {
        const core::Node* node =
            gameState_.level.graph.getNode(piece.currentNodeId);
        if (!node) continue;

        sf::Vector2f piecePos(node->x + fieldOffset_.x,
                              node->y + fieldOffset_.y);
        float dist = std::sqrt(
            (mousePos.x - piecePos.x) * (mousePos.x - piecePos.x) +
            (mousePos.y - piecePos.y) * (mousePos.y - piecePos.y));

        if (dist < hitRadius)
        {
            gameState_.selectedPieceId = piece.id;
            highlightedNodes_ = gameState_.getReachableNodes(piece.id);
            audio_.playSound("click");
            return;
        }
    }

    gameState_.selectedPieceId.reset();
    highlightedNodes_.clear();
}

void GameScreen::handlePieceMove(sf::Vector2f mousePos)
{
    if (!gameState_.selectedPieceId.has_value()) return;

    int pieceId = gameState_.selectedPieceId.value();

    for (int nodeId : highlightedNodes_)
    {
        const core::Node* node = gameState_.level.graph.getNode(nodeId);
        if (!node) continue;

        sf::Vector2f nodePos(node->x + fieldOffset_.x,
                             node->y + fieldOffset_.y);
        float dist = std::sqrt(
            (mousePos.x - nodePos.x) * (mousePos.x - nodePos.x) +
            (mousePos.y - nodePos.y) * (mousePos.y - nodePos.y));

        if (dist < 20.0f)
        {
            MoveCommand cmd;
            cmd.pieceId = pieceId;
            cmd.fromNodeId = gameState_.pieces[pieceId].currentNodeId;
            cmd.toNodeId = nodeId;

            auto waypoints = buildWaypoints(cmd.fromNodeId, cmd.toNodeId);
            cmd.waypoints = waypoints;

            cmd.execute(gameState_);
            moveHistory_.push_back(cmd);

            if (!waypoints.empty())
            {
                tweenManager_.start(pieceId, waypoints);
                animating_ = true;
            }
            audio_.playSound("move");

            gameState_.selectedPieceId.reset();
            highlightedNodes_.clear();
            return;
        }
    }

    gameState_.selectedPieceId.reset();
    highlightedNodes_.clear();
}

std::vector<sf::Vector2f> GameScreen::buildWaypoints(
    int fromNodeId, int toNodeId) const
{
    std::vector<sf::Vector2f> waypoints;

    std::unordered_set<int> occupied;
    for (const auto& p : gameState_.pieces)
    {
        if (p.currentNodeId != fromNodeId)
            occupied.insert(p.currentNodeId);
    }

    auto path = core::PathFinder::bfsPath(
        gameState_.level.graph, fromNodeId, toNodeId, occupied);

    if (path.empty()) return waypoints;

    for (int nodeId : path)
    {
        const core::Node* node = gameState_.level.graph.getNode(nodeId);
        if (node)
            waypoints.push_back(sf::Vector2f(
                node->x + fieldOffset_.x, node->y + fieldOffset_.y));
    }

    return waypoints;
}

void GameScreen::syncSettings()
{
    if (settingsPtr_)
    {
        audio_.setMasterVolume(settingsPtr_->masterVolume);
        audio_.setMusicVolume(settingsPtr_->musicVolume);
        audio_.setSfxVolume(settingsPtr_->sfxVolume);
    }
}

void GameScreen::update(float dt)
{
    syncSettings();
    tweenManager_.update(dt);

    float currentFps = (dt > 0.0f) ? (1.0f / dt) : 0.0f;
    fps_ = fps_ * 0.95f + currentFps * 0.05f;

    for (const auto& tween : tweenManager_.activeTweens)
        renderer_.setPieceScreenPosition(tween.pieceId, tween.currentPosition());
}

void GameScreen::render(sf::RenderTarget& target)
{
    bool dark = settingsPtr_ ? settingsPtr_->darkTheme : true;
    target.clear(dark ? sf::Color(40, 40, 40) : sf::Color(200, 200, 200));
    renderer_.render(gameState_, target, highlightedNodes_,
                     sf::Color(255, 255, 0, 128),
                     20.0f, 15.0f, true, fieldOffset_);
}

void GameScreen::renderImGui()
{
    int lang = settingsPtr_ ? settingsPtr_->language : 0;
    auto _ = [lang](const char* en, const char* ru) { return tr(en, ru, lang); };

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 40.0f));
    ImGui::Begin("Game HUD", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

    if (ImGui::Button(_("Menu", "Меню"), ImVec2(50, 25)))
    {
        audio_.stopMusic();
        std::string savePath = assetsBase_ + "/save.json";
        SaveManager::save(savePath, levelPath_, gameState_, moveHistory_);
        if (onMenu) onMenu();
    }
    ImGui::SameLine();
    if (gameState_.selectedPieceId.has_value())
    {
        ImGui::Text("  %s %d | %s %zu %s",
                    _("Piece", "Фишка"),
                    gameState_.selectedPieceId.value(),
                    _("Reachable:", "Доступно:"),
                    highlightedNodes_.size(),
                    _("nodes", "узлов"));
    }
    else
    {
        ImGui::Text("  %s", _("Click a piece to select",
                              "Нажмите на фишку, чтобы выбрать"));
    }
    ImGui::End();

    if (debugOverlay_)
    {
        ImGui::Begin("Debug Overlay", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("FPS: %.1f", fps_);
        ImGui::Text("Animation: %s", animating_ ? "ACTIVE" : "IDLE");
        ImGui::Text("Pieces: %zu", gameState_.pieces.size());
        ImGui::Text("Move history: %zu", moveHistory_.size());

        if (gameState_.selectedPieceId.has_value())
        {
            auto reachable = gameState_.getReachableNodes(
                gameState_.selectedPieceId.value());
            ImGui::Text("BFS reachable: %zu nodes", reachable.size());
        }
        else
            ImGui::Text("BFS: no piece selected");

        ImGui::Text("Win condition: %s",
                     gameState_.isWinConditionMet() ? "MET" : "not met");
        ImGui::End();
    }
}

bool GameScreen::isDone() const { return done_; }
core::GameState& GameScreen::gameState() { return gameState_; }
TweenManager& GameScreen::tweenManager() { return tweenManager_; }

}
