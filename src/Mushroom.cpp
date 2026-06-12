#include "Mushroom.h"

Mushroom::Mushroom(sf::Vector2f feetPosition)
    : Enemy(feetPosition, "Animations/Enemy/mushroom")
{
}

void Mushroom::update(float deltaTime)
{
    if (updatePassiveState(deltaTime))
    {
        return;
    }

    updateVisuals(deltaTime);
}

void Mushroom::update(float deltaTime, Player&, const std::vector<sf::FloatRect>&)
{
    if (updatePassiveState(deltaTime))
    {
        return;
    }

    updateVisuals(deltaTime);
}
