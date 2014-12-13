#include "Game.h"

#include "PuttyParser.h"
#include "ChatParser.h"

#include <thread>

Game::Game(string path)
{
    isLoaded = false;
    PuttyParser::getInstance().Parse(path, this);
}

Game::~Game()
{
    //dtor
}

Player* Game::CreatePlayer(string ip)
{
    Player* p = new Player();

    p->inventory = new Container();
    p->currentRoom = (startingRoom != "") ? rooms[startingRoom] : rooms.begin()->second;

    ipMap[ip]=p;

    cout << "Player Created!" << endl;

    return p;
}

Player* Game::GetPlayer(string ip)
{
    unique_lock<mutex> lk(ipMutex);

    if (ipMap.find(ip) != ipMap.end())
    {
    return ipMap[ip];
    }
    else
    {
    return CreatePlayer(ip);
    }
}

void Game::Start()
{
    if(isLoaded && !hasError)
    {
        currentRoom = (startingRoom != "") ? rooms[startingRoom] : rooms.begin()->second;
        ipMap = map<string, Player*>();

        //TODO: shouldn't be a pointer
        inventory = new Container();

        ChatParser::Init(this);

        /*cout << ChatParser::Look();

        string uInput;
        while(true)
        {
            cout << endl;
            cout << "> ";
            getline(cin, uInput);
            cout << ChatParser::Parse(uInput);
        }*/
    }
}

Thing* Game::GetItem(string itemName)
{
    return (things)[itemName];
}

Thing* Game::GetItemInRoom(string itemName)
{
    if (currentRoom->contents.count(itemName))
    {
        return currentRoom->contents[itemName];
    }
    else
    {
        for (auto roomPair : currentRoom->contents)
        {
            Thing *roomThing = roomPair.second;
            if (roomThing->isContainer && roomThing->isOpen)
            {
                for (auto innerPair : roomThing->contents)
                {
                    Thing *innerThing = innerPair.second;
                    if (innerThing->contents.count(itemName))
                    {
                        return innerThing->contents[itemName];
                    }
                }
            }
        }
    }
}
