//
// Created by sarai on 20/12/2019.
//

#include "Command.h"
#include "Commands.h"
#include <iostream>
#include <string>

using namespace std;

//Add a command to the commands map
void Commands::add(string key, Command *val) {
    auto it = myMap.find(key);
    if (it == myMap.end()) {
        myMap.insert(pair<string, Command *>(key, val));
    } else {
        this->myMap.at(key) = val;
    }
}

//get a command from map by name
Command *Commands::getCommand(string key) {
    if (myMap.find(key) == myMap.end()) {
        cerr << "key not in map";
        return nullptr;
    } else {
        return myMap.at(key);
    }
}

//initialize map of commands
void Commands::initMap() {
    add("openDataServer", new OpenSeverCommand());
    add("connectControlClient", new ConnectCommand());
    add("var", new DefineVarCommand());
    add("if", new IfCommand());
    add("while", new LoopCommand());
    add("Print", new PrintCommand());
    add("Sleep", new SleepCommand());
    add("=", new AssignCommand());
}