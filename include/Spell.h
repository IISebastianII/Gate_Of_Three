#pragma once

#include <string>

class Spell
{
public:
    Spell(std::string id, float manaCost);

    const std::string& getId() const;
    float getManaCost() const;

private:
    std::string id_;
    float manaCost_ = 0.f;
};
