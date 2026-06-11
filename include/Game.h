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
    void processEvents();
    void update(float deltaTime);
    void render();
    void renderHud();
    void updateCamera();

    sf::RenderWindow window_;
    sf::View gameView_;
    RoomManager roomManager_;
    Player player_;
};
