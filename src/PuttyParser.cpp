#include "PuttyParser.h"

#define IS_DIRECTORY(root) root != NULL
#define IS_ROOM(path) path.rfind("\\area.txt") != string::npos
#define IS_SETTINGS(path) path.rfind("\\settings.txt") != string::npos

PuttyParser PuttyParser::getInstance()
{
    static PuttyParser parser = PuttyParser();
    return parser;
}

void PuttyParser::Parse(string path, Game *game)
{
    ParseSwitch(path, game);
    InflateThings(game);
    InflateRooms(game);
}

void PuttyParser::ParseSwitch(string path, Game *game)
{
    DIR *root = opendir(path.c_str());
    if (IS_DIRECTORY(root))
    {
        ParseChildren(path, game, root);
    }
    else if (IS_SETTINGS(path))
    {
        ParseSettings(path, game);
    }
    else if (IS_ROOM(path))
    {
        ParseRoom(path, game);
    }
    else
    {
        ParseThing(path, game);
    }
}

void PuttyParser::ParseChildren(string path, Game *game, DIR *root)
{
    struct dirent *entry;
    while ((entry = readdir(root)) != NULL)
    {
        if (entry->d_name[0] != '.')
        {
            string subpath = path;
            subpath += "\\";
            subpath += entry->d_name;
            ParseSwitch (subpath, game);
        }
    }
}

void PuttyParser::ParseSettings(string path, Game *game)
{
    fstream in(path);
    string line;
    string word;
    string command = "";
    while (getline(in, line))
    {
        istringstream iss(line);
        while (iss)
        {
            word = GetWord((&iss));
            if (command == "")
                command = word;
            else
            {
                if (command == "start")
                    game->startingRoom = word;
            }

        }
        command = "";
    }
    in.close();
}

void PuttyParser::ParseRoom(string path, Game *game)
{
    string slug = path.substr(0,path.rfind("\\area.txt"));
    slug = slug.substr(slug.rfind("\\")+1);

    Room *room = new Room();
    room->name = BookTitle(slug);

    fstream in(path);
    string line;
    string word;
    string command = "";
    while (getline(in, line))
    {
        istringstream iss(line);
        while (iss)
        {
            word = GetWord((&iss));
            if (command == "")
                command = word;
            else
            {
                if (command == "description")
                    room->description = word;
                else if (command == "contains")
                    room->contentNames.push_back(word);
            }

        }
        command = "";
    }
    game->rooms[slug] = room;
    in.close();
}

void PuttyParser::ParseThing(string path, Game *game)
{
    string slug = path.substr(0,path.rfind(".txt"));
    slug = slug.substr(slug.rfind("\\")+1);

    Thing *thing = new Thing();
    thing->filename = slug;

    fstream in(path);
    string line;
    string word;
    string command = "";
    while (getline(in, line))
    {
        istringstream iss(line);
        while (iss)
        {
            word = GetWord((&iss));
            if (word != "")
            {
                if (command == "")
                {
                    command = word;
                    if (command == "isLocked")
                        thing->isLocked = true;
                    else if (command == "isFree")
                        thing->isFree = true;
                    else if (command == "isReadable")
                        thing->isReadable = true;
                    else if (command == "isAnchored")
                        thing->isAnchored = true;
                    else if (command == "isContainer")
                        thing->isContainer = true;
                }
                else
                {
                    if (command == "keywords")
                        thing->keywords.push_back(word);
                    else if (command == "description")
                        thing->description = word;
                    else if (command == "contains")
                        thing->contentNames.push_back(word);
                    else if (command == "size")
                        thing->size = atoi(word.c_str());
                    else if (command == "capacity")
                        thing->capacity = atoi(word.c_str());
                    else if (command == "name")
                        thing->name += (thing->name == "") ? word : " " + word;
                }
            }
        }
        command = "";
    }
    game->things[slug] = thing;
    in.close();
}

void PuttyParser::InflateThings(Game *game)
{
    for (auto thing : game->things)
    {
        for (auto content : thing.second->contentNames)
        {
            thing.second->contents[content] = game->things[content];
        }
    }
}

void PuttyParser::InflateRooms(Game *game)
{
    for (auto room : game->rooms)
    {
        for (auto content : room.second->contentNames)
        {
            room.second->contents[content] = game->things[content];
        }
    }
}

string PuttyParser::GetWord(istringstream *iss)
{
    string word = "";
    bool inQuotes = false;
    bool isEscaping = false;
    char c;
    while (*iss)
    {
        c = iss->get();
        if (isEscaping)
        {
            if (c == '"')
                word += '"';
            else if (c == '\\')
                word += '\\';
            else if (c == 'n')
                word += '\n';
            isEscaping = false;
        }
        else if (c == '\\')
            isEscaping = true;
        else if (c == '"')
            inQuotes ^= true;
        else if (!IsDelim(c) || inQuotes)
            word += c;
        else if (word != "")
            break;
    }
    return word;
}

string PuttyParser::BookTitle(string original)
{
    string out = "";
    int nextSpace;

    out += toupper(original[0]);
    for (unsigned int i = 1; i < original.length(); i++)
    {
        if (original[i] == ' ')
        {
            nextSpace = original.find(' ', i+1);
            if (nextSpace == -1)
                nextSpace = original.length();
        }
        if (original[i-1] == ' ' && nextSpace - i > 3)
            out += toupper(original[i]);
        else
            out += original[i];
    }
    return out;
}

bool PuttyParser::IsDelim(char c)
{
    vector<char> delims = {':', ' ', ',', static_cast<char>(-1)};
    for (char d : delims)
    {
        if (d == c)
            return true;
    }
    return false;
}
