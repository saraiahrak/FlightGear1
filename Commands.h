//
// Created by sarai on 20/12/2019.
//

#ifndef EX3_COMMANDS_H
#define EX3_COMMANDS_H

#include "Command.h"
#include <iostream>
#include <map>

using namespace std;

class Commands {
public:
    Commands() {
        initMap();
    }

    ~Commands() = default;

    map<string, Command *> myMap;

    void initMap();

    void add(string key, Command *val);

    Command *getCommand(string key);

};

#endif //EX3_COMMANDS_H