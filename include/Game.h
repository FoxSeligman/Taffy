#ifndef GAME_H
#define GAME_H

#include "Room.h"
#include "Thing.h"
#include "Container.h"
#include "Timer.h"
#include "Updatable.h"

#include <map>

class Game : public Updatable
{
    friend class PuttyParser;
    friend class ChatParser;
    public:
        Game(string path);
        virtual ~Game();
        void Start();

    private:
        //Static
        string startingRoom;
        map<string, Room*> rooms;
        map<string, Thing*> things;
        vector<Timer> timers;

        //Dynamic
        Container *inventory;
        Room *currentRoom;
        bool isLoaded;
        bool canAttack = true;

        //Functions
        Thing *GetItem(string itemName);
        Thing *GetItemInRoom(string itemName);

        void lostGame();
};

#endif // GAME_H
