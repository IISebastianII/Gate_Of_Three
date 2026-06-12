#pragma once

#include "Player.h"
#include "Room.h"
#include "RoomManager.h"
#include "TutorialRoom.h"

#include <SFML/Graphics.hpp>

#include <memory>

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
    sf::Font gameOverFont_;
    sf::Texture longBlastIconTexture_;
    sf::Sprite longBlastIconSprite_;
    ScreenState screenState_ = ScreenState::Menu;
    bool hasGameOverFont_ = false;
    bool hasLongBlastIcon_ = false;
};
