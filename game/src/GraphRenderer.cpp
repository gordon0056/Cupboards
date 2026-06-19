#include "GraphRenderer.hpp"
#include <cmath>

namespace game
{

void GraphRenderer::render(const core::GameState& state,
                             sf::RenderTarget& target,
                             const std::vector<int>& highlightedNodes,
                             const sf::Color& highlightColor,
                             float pieceRadius,
                             float nodeRadius,
                             bool showWinTargets,
                             const sf::Vector2f& offset)
{
    std::unordered_set<int> highlightedSet(highlightedNodes.begin(), highlightedNodes.end());

    if (pieceScreenPositions.size() != state.pieces.size())
    {
        pieceScreenPositions.resize(state.pieces.size());
        for (size_t i = 0; i < state.pieces.size(); ++i)
        {
            const core::Node* node = state.level.graph.getNode(state.pieces[i].currentNodeId);
            if (node)
            {
                pieceScreenPositions[i] = sf::Vector2f(
                    node->x + offset.x, node->y + offset.y);
            }
        }
    }

    if (showWinTargets)
    {
        renderWinTargets(state, target, pieceRadius, offset);
    }

    sf::VertexArray lines(sf::Lines, state.level.graph.edges.size() * 2);
    for (size_t i = 0; i < state.level.graph.edges.size(); ++i)
    {
        const core::Edge& edge = state.level.graph.edges[i];
        const core::Node* a = state.level.graph.getNode(edge.a);
        const core::Node* b = state.level.graph.getNode(edge.b);
        if (a && b)
        {
            lines[i * 2].position = sf::Vector2f(a->x + offset.x, a->y + offset.y);
            lines[i * 2].color = sf::Color::White;
            lines[i * 2 + 1].position = sf::Vector2f(b->x + offset.x, b->y + offset.y);
            lines[i * 2 + 1].color = sf::Color::White;
        }
    }
    target.draw(lines);

    for (const auto& node : state.level.graph.nodes)
    {
        sf::CircleShape shape(nodeRadius);
        shape.setOrigin(nodeRadius, nodeRadius);
        shape.setPosition(sf::Vector2f(node.x + offset.x, node.y + offset.y));

        if (highlightedSet.count(node.id) > 0)
        {
            shape.setFillColor(highlightColor);
        }
        else
        {
            shape.setFillColor(sf::Color(100, 100, 100));
        }
        shape.setOutlineThickness(2.0f);
        shape.setOutlineColor(sf::Color::White);
        target.draw(shape);
    }

    for (size_t i = 0; i < state.pieces.size(); ++i)
    {
        const core::Piece& piece = state.pieces[i];
        sf::Vector2f pos = pieceScreenPositions[i];

        sf::CircleShape shape(pieceRadius);
        shape.setOrigin(pieceRadius, pieceRadius);
        shape.setPosition(pos);

        sf::Color color = sf::Color::White;
        if (piece.color.size() >= 7)
        {
            try
            {
                auto parseHex = [](const std::string& s, size_t pos) -> unsigned int
                {
                    return std::stoul(s.substr(pos, 2), nullptr, 16);
                };
                unsigned int r = parseHex(piece.color, 1);
                unsigned int g = parseHex(piece.color, 3);
                unsigned int b = parseHex(piece.color, 5);
                color = sf::Color(static_cast<sf::Uint8>(r),
                                  static_cast<sf::Uint8>(g),
                                  static_cast<sf::Uint8>(b));
            }
            catch (const std::exception&)
            {
                static const sf::Color fallbacks[] = {
                    sf::Color::Red, sf::Color::Green, sf::Color::Blue,
                    sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan,
                };
                color = fallbacks[i % 6];
            }
        }
        else
        {
            static const sf::Color fallbacks[] = {
                sf::Color::Red, sf::Color::Green, sf::Color::Blue,
                sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan,
            };
            color = fallbacks[i % 6];
        }
        shape.setFillColor(color);
        shape.setOutlineThickness(3.0f);
        shape.setOutlineColor(sf::Color::Black);
        target.draw(shape);
    }
}

void GraphRenderer::renderWinTargets(const core::GameState& state,
                                      sf::RenderTarget& target,
                                      float pieceRadius,
                                      const sf::Vector2f& offset)
{
    for (size_t i = 0; i < state.pieces.size() && i < state.level.win_positions.size(); ++i)
    {
        int winNodeId = state.level.win_positions[i];
        const core::Node* node = state.level.graph.getNode(winNodeId);
        if (!node) continue;

        const core::Piece& piece = state.pieces[i];
        sf::Color color(255, 255, 255, 80);
        if (piece.color.size() >= 7)
        {
            try
            {
                auto parseHex = [](const std::string& s, size_t pos) -> unsigned int
                {
                    return std::stoul(s.substr(pos, 2), nullptr, 16);
                };
                unsigned int r = parseHex(piece.color, 1);
                unsigned int g = parseHex(piece.color, 3);
                unsigned int b = parseHex(piece.color, 5);
                color = sf::Color(static_cast<sf::Uint8>(r),
                                  static_cast<sf::Uint8>(g),
                                  static_cast<sf::Uint8>(b), 80);
            }
            catch (const std::exception&)
            {
            }
        }

        sf::CircleShape shape(pieceRadius);
        shape.setOrigin(pieceRadius, pieceRadius);
        shape.setPosition(sf::Vector2f(node->x + offset.x, node->y + offset.y));
        shape.setFillColor(color);
        shape.setOutlineThickness(2.0f);
        shape.setOutlineColor(sf::Color(255, 255, 255, 60));
        target.draw(shape);
    }
}

void GraphRenderer::setPieceScreenPosition(int pieceId, sf::Vector2f screenPos)
{
    if (pieceId >= 0 && pieceId < static_cast<int>(pieceScreenPositions.size()))
    {
        pieceScreenPositions[pieceId] = screenPos;
    }
}

sf::Vector2f GraphRenderer::getPieceScreenPosition(int pieceId) const
{
    if (pieceId >= 0 && pieceId < static_cast<int>(pieceScreenPositions.size()))
    {
        return pieceScreenPositions[pieceId];
    }
    return sf::Vector2f(0, 0);
}

}
