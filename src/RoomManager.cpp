#include "RoomManager.h"

#include "BattleRoom.h"
#include "BossRoom.h"
#include "ChestRoom.h"
#include "HealRoom.h"
#include "TutorialRoom.h"

RoomManager::RoomManager()
{
    changeRoom(RoomType::Tutorial);
}

Room& RoomManager::getCurrentRoom()
{
    return *currentRoom_;
}

const Room& RoomManager::getCurrentRoom() const
{
    return *currentRoom_;
}

void RoomManager::changeRoom(RoomType type)
{
    if (currentRoom_)
    {
        currentRoom_->onExit();
    }

    // A fresh room also resets its enemies and interactive objects.
    currentRoom_ = createRoom(type);
    currentRoom_->onEnter();
}

RoomType RoomManager::getCurrentRoomType() const
{
    return currentRoom_->getType();
}

std::unique_ptr<Room> RoomManager::createRoom(RoomType type) const
{
    switch (type)
    {
    case RoomType::Tutorial:
        return std::make_unique<TutorialRoom>();
    case RoomType::Battle:
        return std::make_unique<BattleRoom>();
    case RoomType::BattleTwo:
        return std::make_unique<BattleRoom>(RoomType::BattleTwo);
    case RoomType::Heal:
        return std::make_unique<HealRoom>();
    case RoomType::Chest:
        return std::make_unique<ChestRoom>();
    case RoomType::Boss:
        return std::make_unique<BossRoom>();
    }

    return std::make_unique<TutorialRoom>();
}
