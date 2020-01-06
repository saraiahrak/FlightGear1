//
// Created by sarai on 20/12/2019.
//

#ifndef EX3_COMMANDS_H
#define EX3_COMMANDS_H

#include "Command.h"
#include <iostream>
#include <map>

using namespace std;


/**
 * Class Commands
 * Holds a map of all possible commands to draw commands from
 *
 **/
class Commands {
public:
    //Constructor
    Commands() {
        //initialize map upon creation
        initMap();
    }

    //Destructor
    ~Commands() = default;

    //Commands map
    map<string, Command *> myMap;


    //Initialize the commands map
    void initMap();

    //add a command to map
    void add(string key, Command *val);


    //get a command from map
    Command *getCommand(string key);

};

#endif //EX3_COMMANDS_H