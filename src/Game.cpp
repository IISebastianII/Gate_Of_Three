#include "Game.h"

#include "AssetPaths.h"

#include <algorithm>

namespace
{
constexpr unsigned windowWidth = 1280;
constexpr unsigned windowHeight = 720;
constexpr float maxDeltaTime = 1.f / 30.f;
constexpr float menuPanelWidth = 760.f;
constexpr float menuPanelHeight = 468.f;
constexpr float menuButtonWidth = 320.f;
constexpr float menuButtonHeight = 72.f;
constexpr float menuButtonGap = 18.f;
constexpr float menuButtonTop = 322.f;
constexpr float menuTitleTop = 170.f;
constexpr float menuPanelTop = 128.f;
constexpr float menuTitleBarWidth = 460.f;
constexpr float menuTitleBarHeight = 90.f;
constexpr float healthBarWidth = 220.f;
constexpr float healthBarHeight = 12.f;
constexpr float healthBarPadding = 24.f;
constexpr float manaBarGap = 6.f;
constexpr float iconGap = 14.f;
constexpr float iconFramePadding = 3.f;
constexpr float retryButtonWidth = 320.f;
constexpr float retryButtonHeight = 56.f;
constexpr float interactionPaddingX = 36.f;
constexpr float interactionPaddingY = 28.f;

sf::FloatRect expandBounds(const sf::FloatRect& bounds, float paddingX, float paddingY)
{
    return {
        bounds.left - paddingX,
        bounds.top - paddingY,
        bounds.width + paddingX * 2.f,
        bounds.height + paddingY * 2.f
    };
}

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

    const auto loadTexture = [](sf::Texture& texture, const std::string& path)
    {
        const bool loaded = texture.loadFromFile(AssetPaths::resolve(path).string());
        texture.setSmooth(false);
        return loaded;
    };

    loadTexture(menuBackgroundTexture_, "backgrounds/forest_background.png");
    loadTexture(menuPanelTexture_, "UI Package/Frames/1-1.png");
    loadTexture(menuTitleTexture_, "UI Package/Windows/5-1.png");
    loadTexture(menuButtonNormalTexture_, "UI Package/Elements/5-1.png");
    loadTexture(menuButtonHoverTexture_, "UI Package/Elements/5-2.png");
    loadTexture(menuButtonPressedTexture_, "UI Package/Elements/5-3.png");

    sf::Image longBlastIconImage;
    hasLongBlastIcon_ = longBlastIconImage.loadFromFile(AssetPaths::resolve("long_blast_icon.png").string());
    if (hasLongBlastIcon_)
    {
        hasLongBlastIcon_ = longBlastIconTexture_.loadFromImage(longBlastIconImage);
        longBlastIconTexture_.setSmooth(false);
        longBlastIconSprite_.setTexture(longBlastIconTexture_);
    }
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

        if (screenState_ == ScreenState::Menu)
        {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                const sf::Vector2f mousePosition = window_.mapPixelToCoords(
                    {event.mouseButton.x, event.mouseButton.y},
                    window_.getDefaultView());

                if (getPlayButtonBounds().contains(mousePosition))
                {
                    startGame();
                }
                else if (getExitButtonBounds().contains(mousePosition))
                {
                    window_.close();
                }
            }

            continue;
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

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E && !player_.isDead())
        {
            Room& room = roomManager_.getCurrentRoom();
            if (const RoomExit* exit = room.findTouchedExit(player_.getBounds()))
            {
                const RoomType targetRoom = exit->getTargetRoom();
                const sf::Vector2f targetSpawnFeet = exit->getTargetSpawnFeet();
                roomManager_.changeRoom(targetRoom);
                player_.setFeetPosition(targetSpawnFeet);
            }
            else
            {
                room.interactObjectsInBounds(expandBounds(player_.getBounds(), interactionPaddingX, interactionPaddingY));
            }
        }

        player_.handleEvent(event);
    }
}

void Game::update(float deltaTime)
{
    if (screenState_ == ScreenState::Menu)
    {
        return;
    }

    Room& room = roomManager_.getCurrentRoom();
    player_.update(deltaTime, room.getSolidColliders(), room.getBounds());
    if (player_.consumeSpellCastRequest())
    {
        room.tryCastSpell(player_);
    }
    if (player_.consumeSpellProjectileSpawnRequest())
    {
        const Spell& spell = player_.getLongBlastSpell();
        room.spawnProjectile(
            player_.getSpellSpawnPosition(),
            player_.getFacingDirection(),
            spell.getProjectileSpeed(),
            spell.getDamage());
    }
    room.update(deltaTime, player_);
    if (room.consumeLongBlastUnlockRequest())
    {
        player_.unlockLongBlastSpell();
    }
    if (player_.isAttackActive())
    {
        room.damageObjectsInBounds(player_.getAttackBounds(), player_.getAttackDamage(), player_.getCenter());
    }
    updateCamera();
}

void Game::render()
{
    window_.clear(sf::Color(126, 184, 208));
    if (screenState_ == ScreenState::Menu)
    {
        renderMenu();
    }
    else
    {
        window_.setView(gameView_);

        roomManager_.getCurrentRoom().draw(window_);
        player_.draw(window_);

        renderHud();
    }

    window_.display();
}

void Game::renderMenu()
{
    window_.setView(window_.getDefaultView());

    const sf::Vector2f windowSize(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    if (menuBackgroundTexture_.getSize().x > 0 && menuBackgroundTexture_.getSize().y > 0)
    {
        sf::Sprite backgroundSprite(menuBackgroundTexture_);
        const sf::Vector2u backgroundSize = menuBackgroundTexture_.getSize();
        backgroundSprite.setScale(
            windowSize.x / static_cast<float>(backgroundSize.x),
            windowSize.y / static_cast<float>(backgroundSize.y));
        window_.draw(backgroundSprite);
    }
    else
    {
        sf::RectangleShape backdrop(windowSize);
        backdrop.setFillColor(sf::Color(22, 28, 40));
        window_.draw(backdrop);
    }

    sf::RectangleShape overlay(windowSize);
    overlay.setFillColor(sf::Color(10, 14, 22, 150));
    window_.draw(overlay);

    if (menuPanelTexture_.getSize().x > 0 && menuPanelTexture_.getSize().y > 0)
    {
        sf::Sprite panelSprite(menuPanelTexture_);
        const sf::Vector2u panelSize = menuPanelTexture_.getSize();
        panelSprite.setScale(
            menuPanelWidth / static_cast<float>(panelSize.x),
            menuPanelHeight / static_cast<float>(panelSize.y));
        panelSprite.setPosition(
            (windowSize.x - menuPanelWidth) * 0.5f,
            menuPanelTop);
        window_.draw(panelSprite);
    }
    else
    {
        sf::RectangleShape panel({menuPanelWidth, menuPanelHeight});
        panel.setPosition((windowSize.x - menuPanelWidth) * 0.5f, menuPanelTop);
        panel.setFillColor(sf::Color(10, 14, 22, 170));
        panel.setOutlineColor(sf::Color(255, 255, 255, 50));
        panel.setOutlineThickness(2.f);
        window_.draw(panel);
    }

    if (menuTitleTexture_.getSize().x > 0 && menuTitleTexture_.getSize().y > 0)
    {
        sf::Sprite titleBar(menuTitleTexture_);
        const sf::Vector2u titleSize = menuTitleTexture_.getSize();
        titleBar.setScale(
            menuTitleBarWidth / static_cast<float>(titleSize.x),
            menuTitleBarHeight / static_cast<float>(titleSize.y));
        titleBar.setPosition(
            (windowSize.x - menuTitleBarWidth) * 0.5f,
            menuTitleTop);
        window_.draw(titleBar);
    }

    if (hasGameOverFont_)
    {
        sf::Text title;
        title.setFont(gameOverFont_);
        title.setString("Gate of Three");
        title.setCharacterSize(78);
        title.setFillColor(sf::Color(248, 244, 232));
        title.setOutlineColor(sf::Color(34, 20, 30));
        title.setOutlineThickness(4.f);

        const sf::FloatRect titleBounds = title.getLocalBounds();
        title.setOrigin(
            titleBounds.left + titleBounds.width * 0.5f,
            titleBounds.top + titleBounds.height * 0.5f);
        title.setPosition(windowSize.x * 0.5f, menuTitleTop + menuTitleBarHeight * 0.48f);
        window_.draw(title);

        const sf::Vector2f mousePosition = window_.mapPixelToCoords(
            sf::Mouse::getPosition(window_),
            window_.getDefaultView());

        const auto drawButton = [&](const sf::FloatRect& bounds, const sf::Texture* normalTexture, const sf::Texture* hoverTexture, const sf::Texture* pressedTexture, const sf::String& label)
        {
            const bool hovered = bounds.contains(mousePosition);
            const bool pressed = hovered && sf::Mouse::isButtonPressed(sf::Mouse::Left);
            const sf::Texture* texture = pressed
                ? pressedTexture
                : hovered
                    ? hoverTexture
                    : normalTexture;

            if (texture != nullptr && texture->getSize().x > 0 && texture->getSize().y > 0)
            {
                sf::Sprite buttonSprite(*texture);
                const sf::Vector2u textureSize = texture->getSize();
                buttonSprite.setPosition(bounds.left, bounds.top);
                buttonSprite.setScale(
                    bounds.width / static_cast<float>(textureSize.x),
                    bounds.height / static_cast<float>(textureSize.y));
                window_.draw(buttonSprite);
            }
            else
            {
                sf::RectangleShape button({bounds.width, bounds.height});
                button.setPosition(bounds.left, bounds.top);
                button.setFillColor(hovered ? sf::Color(60, 74, 96) : sf::Color(35, 43, 56));
                button.setOutlineColor(sf::Color(255, 255, 255, hovered ? 130 : 80));
                button.setOutlineThickness(2.f);
                window_.draw(button);
            }

            sf::Text text;
            text.setFont(gameOverFont_);
            text.setString(label);
            text.setCharacterSize(32);
            text.setFillColor(sf::Color(248, 245, 239));
            text.setOutlineColor(sf::Color(33, 21, 28));
            text.setOutlineThickness(2.f);

            const sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(
                textBounds.left + textBounds.width * 0.5f,
                textBounds.top + textBounds.height * 0.5f);
            text.setPosition(
                bounds.left + bounds.width * 0.5f,
                bounds.top + bounds.height * 0.5f - 3.f);
            window_.draw(text);
        };

        drawButton(
            getPlayButtonBounds(),
            &menuButtonNormalTexture_,
            &menuButtonHoverTexture_,
            &menuButtonPressedTexture_,
            "Graj");
        drawButton(
            getExitButtonBounds(),
            &menuButtonNormalTexture_,
            &menuButtonHoverTexture_,
            &menuButtonPressedTexture_,
            L"Wyjd\u017A");
    }
    else
    {
        const sf::Vector2f mousePosition = window_.mapPixelToCoords(
            sf::Mouse::getPosition(window_),
            window_.getDefaultView());

        const auto drawMenuButton = [&](const sf::FloatRect& bounds, const sf::String& label, bool highlighted)
        {
            sf::RectangleShape button({bounds.width, bounds.height});
            button.setPosition(bounds.left, bounds.top);
            button.setFillColor(highlighted ? sf::Color(60, 74, 96) : sf::Color(35, 43, 56));
            button.setOutlineColor(sf::Color(255, 255, 255, highlighted ? 130 : 80));
            button.setOutlineThickness(2.f);
            window_.draw(button);

            sf::Text text;
            text.setFont(gameOverFont_);
            text.setString(label);
            text.setCharacterSize(34);
            text.setFillColor(sf::Color::White);

            const sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(
                textBounds.left + textBounds.width * 0.5f,
                textBounds.top + textBounds.height * 0.5f);
            text.setPosition(
                bounds.left + bounds.width * 0.5f,
                bounds.top + bounds.height * 0.5f - 2.f);
            window_.draw(text);
        };

        drawMenuButton(getPlayButtonBounds(), "Graj", getPlayButtonBounds().contains(mousePosition));
        drawMenuButton(getExitButtonBounds(), L"Wyjd\u017A", getExitButtonBounds().contains(mousePosition));
    }
}

void Game::renderHud()
{
    window_.setView(window_.getDefaultView());

    const float maxHealth = static_cast<float>(std::max(1, player_.getMaxHealth()));
    const float healthRatio = std::clamp(static_cast<float>(player_.getHealth()) / maxHealth, 0.f, 1.f);
    const float manaRatio = static_cast<float>(player_.getMana()) / static_cast<float>(std::max(1, player_.getMaxMana()));

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

    if (hasLongBlastIcon_)
    {
        const sf::Vector2u textureSize = longBlastIconTexture_.getSize();
        if (textureSize.x > 0 && textureSize.y > 0)
        {
            const float iconHeight = healthBarHeight * 2.f + manaBarGap + 6.f;
            const float scale = iconHeight / static_cast<float>(textureSize.y);
            const float iconWidth = static_cast<float>(textureSize.x) * scale;
            const float frameWidth = iconWidth + iconFramePadding * 2.f;
            const float frameHeight = iconHeight + iconFramePadding * 2.f;
            const float frameLeft = healthBarPadding + healthBarWidth + iconGap - iconFramePadding;
            const float frameTop = healthBarPadding - 3.f - iconFramePadding;

            sf::RectangleShape iconFrame({frameWidth, frameHeight});
            iconFrame.setPosition(frameLeft, frameTop);
            iconFrame.setFillColor(sf::Color(20, 24, 32, 170));
            iconFrame.setOutlineThickness(1.f);
            iconFrame.setOutlineColor(sf::Color(255, 255, 255, 65));
            window_.draw(iconFrame);

            longBlastIconSprite_.setPosition(
                healthBarPadding + healthBarWidth + iconGap,
                healthBarPadding - 3.f);
            longBlastIconSprite_.setColor(player_.hasLongBlastUnlocked()
                ? sf::Color(245, 245, 245, 255)
                : sf::Color(150, 150, 150, 170));
            longBlastIconSprite_.setScale(scale, scale);
            window_.draw(longBlastIconSprite_);
        }
    }

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

sf::FloatRect Game::getPlayButtonBounds() const
{
    return {
        (static_cast<float>(windowWidth) - menuButtonWidth) * 0.5f,
        menuButtonTop,
        menuButtonWidth,
        menuButtonHeight
    };
}

sf::FloatRect Game::getExitButtonBounds() const
{
    return {
        (static_cast<float>(windowWidth) - menuButtonWidth) * 0.5f,
        menuButtonTop + menuButtonHeight + menuButtonGap,
        menuButtonWidth,
        menuButtonHeight
    };
}

void Game::startGame()
{
    screenState_ = ScreenState::Playing;
    player_.setFeetPosition(roomManager_.getCurrentRoom().getPlayerSpawnFeet());
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
