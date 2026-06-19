#include "TweenManager.hpp"
#include <cmath>
#include <algorithm>

namespace game
{

static sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t)
{
    return a + (b - a) * t;
}

sf::Vector2f TweenAnimation::currentPosition() const
{
    if (currentWaypointIndex == 0) return waypoints[0];
    if (currentWaypointIndex >= static_cast<int>(waypoints.size())) return waypoints.back();
    return lerp(waypoints[currentWaypointIndex - 1],
                waypoints[currentWaypointIndex], progress);
}

void TweenManager::start(int pieceId, const std::vector<sf::Vector2f>& waypoints, float speed)
{
    if (waypoints.empty()) return;

    TweenAnimation anim;
    anim.pieceId = pieceId;
    anim.waypoints = waypoints;
    anim.currentWaypointIndex = 1;
    anim.progress = 0.0f;
    anim.speed = speed;
    anim.isComplete = false;
    activeTweens.push_back(anim);
}

void TweenManager::update(float dt)
{
    for (auto& anim : activeTweens)
    {
        if (anim.isComplete) continue;
        if (anim.waypoints.size() < 2)
        {
            anim.isComplete = true;
            continue;
        }

        sf::Vector2f from = anim.waypoints[anim.currentWaypointIndex - 1];
        sf::Vector2f to = anim.waypoints[anim.currentWaypointIndex];
        float segmentLength = std::sqrt(
            (to.x - from.x) * (to.x - from.x) +
            (to.y - from.y) * (to.y - from.y));

        if (segmentLength == 0.0f)
        {
            anim.currentWaypointIndex++;
            anim.progress = 0.0f;
            if (anim.currentWaypointIndex >= static_cast<int>(anim.waypoints.size()))
                anim.isComplete = true;
            continue;
        }

        float distanceToTravel = anim.speed * dt;
        float progressDelta = distanceToTravel / segmentLength;
        anim.progress += progressDelta;

        while (anim.progress >= 1.0f && !anim.isComplete)
        {
            anim.progress -= 1.0f;
            anim.currentWaypointIndex++;
            if (anim.currentWaypointIndex >= static_cast<int>(anim.waypoints.size()))
            {
                anim.isComplete = true;
                anim.progress = 0.0f;
                break;
            }
            from = anim.waypoints[anim.currentWaypointIndex - 1];
            to = anim.waypoints[anim.currentWaypointIndex];
            segmentLength = std::sqrt(
                (to.x - from.x) * (to.x - from.x) +
                (to.y - from.y) * (to.y - from.y));
            if (segmentLength > 0.0f)
            {
                progressDelta = distanceToTravel / segmentLength;
                anim.progress = progressDelta;
                break;
            }
        }
    }

    bool hadCompletion = false;
    activeTweens.erase(
        std::remove_if(activeTweens.begin(), activeTweens.end(),
            [&hadCompletion](const TweenAnimation& a)
            {
                if (a.isComplete) { hadCompletion = true; return true; }
                return false;
            }),
        activeTweens.end());

    if (hadCompletion && onCompleteCallback)
        onCompleteCallback();
}

bool TweenManager::isAnimating() const
{
    return !activeTweens.empty();
}

}
