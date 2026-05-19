#pragma once

#include "Player.h"
#include "TutorialRoom.h"

#include <SFML/Graphics.hpp>

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
    TutorialRoom tutorialRoom_;
    Player player_;
};
