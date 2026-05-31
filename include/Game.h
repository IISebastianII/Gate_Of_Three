#pragma once

#include "Player.h"
#include "Room.h"
#include "TutorialRoom.h"

#include <SFML/Graphics.hpp>

#include <memory>

class Game
{
public:
    Game();

    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void updateCamera();

    sf::RenderWindow window_;
    sf::View gameView_;
    std::unique_ptr<Room> currentRoom_;
    Player player_;
};
