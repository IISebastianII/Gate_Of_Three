#include "Game.h"

#include <algorithm>

namespace
{
constexpr unsigned windowWidth = 1280;
constexpr unsigned windowHeight = 720;
constexpr float maxDeltaTime = 1.f / 30.f;
constexpr float healthBarWidth = 220.f;
constexpr float healthBarHeight = 24.f;
constexpr float healthBarPadding = 24.f;
}

Game::Game()
    : window_(sf::VideoMode(windowWidth, windowHeight), "Gate of Three")
    , gameView_(sf::FloatRect(0.f, 0.f, static_cast<float>(windowWidth), static_cast<float>(windowHeight)))
{
    window_.setFramerateLimit(120);
    window_.setVerticalSyncEnabled(true);
    window_.setKeyRepeatEnabled(false);

    player_.setFeetPosition(roomManager_.getCurrentRoom().getPlayerSpawnFeet());
    updateCamera();
}

void Game::run()
{
    sf::Clock clock;

    while (window_.isOpen())
    {
        const float deltaTime = std::min(clock.restart().asSeconds(), maxDeltaTime);
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents()
{
    sf::Event event{};
    while (window_.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window_.close();
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            window_.close();
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E)
        {
            Room& room = roomManager_.getCurrentRoom();
            if (const RoomExit* exit = room.findTouchedExit(player_.getBounds()))
            {
                const RoomType targetRoom = exit->getTargetRoom();
                const sf::Vector2f targetSpawnFeet = exit->getTargetSpawnFeet();
                roomManager_.changeRoom(targetRoom);
                player_.setFeetPosition(targetSpawnFeet);
            }
        }

        player_.handleEvent(event);
    }
}

void Game::update(float deltaTime)
{
    Room& room = roomManager_.getCurrentRoom();
    player_.update(deltaTime, room.getSolidColliders(), room.getBounds());
    room.update(deltaTime, player_);
    if (player_.isAttackActive())
    {
        room.damageObjectsInBounds(player_.getAttackBounds(), player_.getAttackDamage(), player_.getCenter());
    }
    updateCamera();
}

void Game::render()
{
    window_.clear(sf::Color(126, 184, 208));
    window_.setView(gameView_);

    roomManager_.getCurrentRoom().draw(window_);
    player_.draw(window_);

    renderHud();

    window_.display();
}

void Game::renderHud()
{
    window_.setView(window_.getDefaultView());

    sf::RectangleShape border({healthBarWidth + 4.f, healthBarHeight + 4.f});
    border.setPosition(healthBarPadding - 2.f, healthBarPadding - 2.f);
    border.setFillColor(sf::Color(20, 20, 20));
    window_.draw(border);

    sf::RectangleShape background({healthBarWidth, healthBarHeight});
    background.setPosition(healthBarPadding, healthBarPadding);
    background.setFillColor(sf::Color(80, 80, 80));
    window_.draw(background);

    const float maxHealth = static_cast<float>(std::max(1, player_.getMaxHealth()));
    const float healthRatio = std::clamp(static_cast<float>(player_.getHealth()) / maxHealth, 0.f, 1.f);

    sf::RectangleShape fill({healthBarWidth * healthRatio, healthBarHeight});
    fill.setPosition(healthBarPadding, healthBarPadding);
    fill.setFillColor(sf::Color(190, 35, 42));
    window_.draw(fill);
}

void Game::updateCamera()
{
    const sf::FloatRect roomBounds = roomManager_.getCurrentRoom().getBounds();
    const sf::Vector2f halfView = gameView_.getSize() * 0.5f;
    const sf::Vector2f playerCenter = player_.getCenter();

    const float minX = roomBounds.left + halfView.x;
    const float maxX = roomBounds.left + roomBounds.width - halfView.x;
    const float minY = roomBounds.top + halfView.y;
    const float maxY = roomBounds.top + roomBounds.height - halfView.y;

    const float cameraX = roomBounds.width <= gameView_.getSize().x
        ? roomBounds.left + roomBounds.width * 0.5f
        : std::clamp(playerCenter.x, minX, maxX);

    const float cameraY = roomBounds.height <= gameView_.getSize().y
        ? roomBounds.top + roomBounds.height * 0.5f
        : std::clamp(playerCenter.y, minY, maxY);

    gameView_.setCenter(cameraX, cameraY);
}
