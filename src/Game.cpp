#include "Game.h"

#include <algorithm>

namespace
{
constexpr unsigned windowWidth = 1280;
constexpr unsigned windowHeight = 720;
constexpr float maxDeltaTime = 1.f / 30.f;
constexpr float healthBarWidth = 220.f;
constexpr float healthBarHeight = 12.f;
constexpr float healthBarPadding = 24.f;
constexpr float manaBarGap = 6.f;
constexpr float retryButtonWidth = 320.f;
constexpr float retryButtonHeight = 56.f;
}

Game::Game()
    : window_(sf::VideoMode(windowWidth, windowHeight), "Gate of Three")
    , gameView_(sf::FloatRect(0.f, 0.f, static_cast<float>(windowWidth), static_cast<float>(windowHeight)))
{
    window_.setFramerateLimit(120);
    window_.setVerticalSyncEnabled(true);
    window_.setKeyRepeatEnabled(false);

    player_.setFeetPosition(roomManager_.getCurrentRoom().getPlayerSpawnFeet());
    hasGameOverFont_ = gameOverFont_.loadFromFile("C:/Windows/Fonts/arial.ttf");
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

        if (event.type == sf::Event::MouseButtonPressed
            && event.mouseButton.button == sf::Mouse::Left
            && player_.isDead())
        {
            const sf::Vector2f mousePosition = window_.mapPixelToCoords(
                {event.mouseButton.x, event.mouseButton.y},
                window_.getDefaultView());

            if (getRetryButtonBounds().contains(mousePosition))
            {
                restartGame();
                continue;
            }
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

    const float maxHealth = static_cast<float>(std::max(1, player_.getMaxHealth()));
    const float healthRatio = std::clamp(static_cast<float>(player_.getHealth()) / maxHealth, 0.f, 1.f);
    const float manaRatio = 1.f;

    const auto drawBar = [&](float top, float ratio, const sf::Color& fillColor)
    {
        sf::RectangleShape border({healthBarWidth + 4.f, healthBarHeight + 4.f});
        border.setPosition(healthBarPadding - 2.f, top - 2.f);
        border.setFillColor(sf::Color(20, 20, 20));
        window_.draw(border);

        sf::RectangleShape background({healthBarWidth, healthBarHeight});
        background.setPosition(healthBarPadding, top);
        background.setFillColor(sf::Color(80, 80, 80));
        window_.draw(background);

        sf::RectangleShape fill({healthBarWidth * ratio, healthBarHeight});
        fill.setPosition(healthBarPadding, top);
        fill.setFillColor(fillColor);
        window_.draw(fill);
    };

    drawBar(healthBarPadding, healthRatio, sf::Color(190, 35, 42));
    drawBar(healthBarPadding + healthBarHeight + manaBarGap, manaRatio, sf::Color(150, 110, 255));

    if (player_.isDead() && hasGameOverFont_)
    {
        sf::Text gameOverText;
        gameOverText.setFont(gameOverFont_);
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(72);
        gameOverText.setFillColor(sf::Color(210, 35, 42));
        gameOverText.setOutlineColor(sf::Color(15, 15, 15));
        gameOverText.setOutlineThickness(4.f);

        const sf::FloatRect bounds = gameOverText.getLocalBounds();
        gameOverText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
        gameOverText.setPosition(static_cast<float>(windowWidth) * 0.5f, static_cast<float>(windowHeight) * 0.5f);
        window_.draw(gameOverText);

        const sf::FloatRect buttonBounds = getRetryButtonBounds();
        sf::RectangleShape retryButton({buttonBounds.width, buttonBounds.height});
        retryButton.setPosition(buttonBounds.left, buttonBounds.top);
        retryButton.setFillColor(sf::Color(35, 35, 35));
        retryButton.setOutlineColor(sf::Color(230, 230, 230));
        retryButton.setOutlineThickness(3.f);
        window_.draw(retryButton);

        sf::Text retryText;
        retryText.setFont(gameOverFont_);
        retryText.setString("SPROBUJ PONOWNIE");
        retryText.setCharacterSize(28);
        retryText.setFillColor(sf::Color::White);

        const sf::FloatRect retryTextBounds = retryText.getLocalBounds();
        retryText.setOrigin(
            retryTextBounds.left + retryTextBounds.width * 0.5f,
            retryTextBounds.top + retryTextBounds.height * 0.5f);
        retryText.setPosition(
            buttonBounds.left + buttonBounds.width * 0.5f,
            buttonBounds.top + buttonBounds.height * 0.5f);
        window_.draw(retryText);
    }
}

void Game::restartGame()
{
    const RoomType currentRoomType = roomManager_.getCurrentRoomType();
    roomManager_.changeRoom(currentRoomType);
    player_.resetForRestart(roomManager_.getCurrentRoom().getPlayerSpawnFeet());
    updateCamera();
}

sf::FloatRect Game::getRetryButtonBounds() const
{
    return {
        (static_cast<float>(windowWidth) - retryButtonWidth) * 0.5f,
        static_cast<float>(windowHeight) * 0.5f + 80.f,
        retryButtonWidth,
        retryButtonHeight
    };
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
