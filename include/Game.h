#pragma once

#include "Player.h"
#include "RoomManager.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class Game
{
public:
    Game();

    void run();

private:
    enum class ScreenState
    {
        Menu,
        Playing
    };

    void processEvents();
    void update(float deltaTime);
    void render();
    void renderMenu();
    void renderHud();
    void updateCamera();
    void restartGame();
    sf::FloatRect getRetryButtonBounds() const;
    sf::FloatRect getPlayButtonBounds() const;
    sf::FloatRect getExitButtonBounds() const;
    void startGame();

    sf::RenderWindow window_;
    sf::View gameView_;
    RoomManager roomManager_;
    Player player_;
    sf::Texture menuBackgroundTexture_;
    sf::Texture menuPanelTexture_;
    sf::Texture menuTitleTexture_;
    sf::Texture menuButtonNormalTexture_;
    sf::Texture menuButtonHoverTexture_;
    sf::Texture menuButtonPressedTexture_;
    sf::Font gameOverFont_;
    sf::Texture longBlastIconTexture_;
    sf::Sprite longBlastIconSprite_;
    sf::Music backgroundMusic_;
    ScreenState screenState_ = ScreenState::Menu;
    bool hasGameOverFont_ = false;
    bool hasLongBlastIcon_ = false;
};
