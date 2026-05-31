#include "Spell.h"

#include <utility>

Spell::Spell(std::string id, float manaCost)
    : id_(std::move(id))
    , manaCost_(manaCost)
{
}

const std::string& Spell::getId() const
{
    return id_;
}

float Spell::getManaCost() const
{
    return manaCost_;
}
