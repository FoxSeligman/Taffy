#ifndef THING_H
#define THING_H

#include <map>
#include <string>
#include <vector>
#include "Timer.h"
#include "Updatable.h"

using namespace std;

class Thing : public Updatable
{
    friend class PuttyParser;
    public:
        Thing();
        virtual ~Thing();

        //Static
        string appearance;
        string name;
        string filename;
        vector<string> keywords;
        string description;
        int size = -1;
        int capacity = 0;
        vector<Timer> timers;

        //Dynamic
        map<string, Thing*> contents;
        bool isOpen = false;
        bool isLocked = false;
        bool isFree = false;
        bool isReadable = false;
        bool isAnchored = false;
        bool isContainer = false;
        bool isBreakable = false;

        //Functions
        void Look();
        string GetName();
        string ArticleName();

    private:
        vector<string> contentNames;
};

#endif // THING_H
