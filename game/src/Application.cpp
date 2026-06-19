#include "Application.hpp"

namespace game
{

void Application::pushScreen(std::unique_ptr<IScreen> screen)
{
    pendingPushes_.push_back(std::move(screen));
}

void Application::popScreen()
{
    pendingPopCount_++;
}

void Application::popToBottom()
{
    if (!screens_.empty())
    {
        pendingPopCount_ = static_cast<int>(screens_.size()) - 1;
    }
}

void Application::applyPending()
{
    while (pendingPopCount_ > 0 && !screens_.empty())
    {
        screens_.pop_back();
        pendingPopCount_--;
    }
    for (auto& s : pendingPushes_)
    {
        screens_.push_back(std::move(s));
    }
    pendingPushes_.clear();
}

void Application::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (!screens_.empty())
    {
        screens_.back()->handleEvent(event, window);
    }
}

void Application::update(float dt)
{
    applyPending();
    if (!screens_.empty())
    {
        screens_.back()->update(dt);
        if (screens_.back()->isDone())
        {
            pendingPopCount_++;
        }
    }
    applyPending();
}

void Application::render(sf::RenderTarget& target)
{
    if (!screens_.empty())
    {
        screens_.back()->render(target);
    }
}

void Application::renderImGui()
{
    if (!screens_.empty())
    {
        screens_.back()->renderImGui();
    }
    applyPending();
}

bool Application::isRunning() const
{
    return !screens_.empty();
}

}
