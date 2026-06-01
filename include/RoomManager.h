#pragma once

#include "Room.h"
#include "RoomType.h"

#include <memory>

class RoomManager
{
public:
    RoomManager();

    Room& getCurrentRoom();
    const Room& getCurrentRoom() const;
    void changeRoom(RoomType type);
    RoomType getCurrentRoomType() const;

private:
    std::unique_ptr<Room> createRoom(RoomType type) const;

    std::unique_ptr<Room> currentRoom_;
};
