//
// Created by sarai on 20/12/2019.
//
#include "Initializer.h"
#include <string>
#include "iostream"
#include "Command.h"
#include "Commands.h"
#include "Utils.h"
#include <vector>
#include <fstream>
#include <thread>

using namespace std;

/**
 * Initializer class functions
 **/
bool Initializer::stop;

map<string, float>Initializer::variableValue;

map<string, string>Initializer::variablePath;

map<string, bool> Initializer::variableDirection;

vector<string> Initializer::xmlVals;

mutex Initializer::valLock;

mutex Initializer::pathLock;

mutex Initializer::directLock;

/*
data flow analysis
*/
Initializer::Initializer() {
    initializeXML();
    setStop(false);
}

/*
Creates a vector of strings for all the possible variables paths
*/
void Initializer::initializeXML() {
    // by the generic_small file
    xmlVals.emplace_back("/instrumentation/airspeed-indicator/indicated-speed-kt");
    xmlVals.emplace_back("/sim/time/warp");
    xmlVals.emplace_back("/controls/switches/magnetos");
    xmlVals.emplace_back("/instrumentation/heading-indicator/offset-deg");
    xmlVals.emplace_back("/instrumentation/altimeter/indicated-altitude-ft");
    xmlVals.emplace_back("/instrumentation/altimeter/pressure-alt-ft");
    xmlVals.emplace_back("/instrumentation/attitude-indicator/indicated-pitch-deg");
    xmlVals.emplace_back("/instrumentation/attitude-indicator/indicated-roll-deg");
    xmlVals.emplace_back("/instrumentation/attitude-indicator/internal-pitch-deg");
    xmlVals.emplace_back("/instrumentation/attitude-indicator/internal-roll-deg");
    xmlVals.emplace_back("/instrumentation/encoder/indicated-altitude-ft");
    xmlVals.emplace_back("/instrumentation/encoder/pressure-alt-ft");
    xmlVals.emplace_back("/instrumentation/gps/indicated-altitude-ft");
    xmlVals.emplace_back("/instrumentation/gps/indicated-ground-speed-kt");
    xmlVals.emplace_back("/instrumentation/gps/indicated-vertical-speed");
    xmlVals.emplace_back("/instrumentation/heading-indicator/indicated-heading-deg");
    xmlVals.emplace_back("/instrumentation/magnetic-compass/indicated-heading-deg");
    xmlVals.emplace_back("/instrumentation/slip-skid-ball/indicated-slip-skid");
    xmlVals.emplace_back("/instrumentation/turn-indicator/indicated-turn-rate");
    xmlVals.emplace_back("/instrumentation/vertical-speed-indicator/indicated-speed-fpm");
    xmlVals.emplace_back("/controls/flight/aileron");
    xmlVals.emplace_back("/controls/flight/elevator");
    xmlVals.emplace_back("/controls/flight/rudder");
    xmlVals.emplace_back("/controls/flight/flaps");
    xmlVals.emplace_back("/controls/engines/engine/throttle");
    xmlVals.emplace_back("/controls/engines/current-engine/throttle");
    xmlVals.emplace_back("/controls/switches/master-avionics");
    xmlVals.emplace_back("/controls/switches/starter");
    xmlVals.emplace_back("/engines/active-engine/auto-start");
    xmlVals.emplace_back("/controls/flight/speedbrake");
    xmlVals.emplace_back("/sim/model/c172p/brake-parking");
    xmlVals.emplace_back("/controls/engines/engine/primer");
    xmlVals.emplace_back("/controls/engines/current-engine/mixture");
    xmlVals.emplace_back("/controls/switches/master-bat");
    xmlVals.emplace_back("/controls/switches/master-alt");
    xmlVals.emplace_back("/engines/engine/rpm");
}

/*
set stop
*/
void Initializer::setStop(bool s) {
    Initializer::stop = s;
}

/*
Insert the string into the parameters vector of the program while
the given string is separate to his name and value
*/
vector<string> Initializer::isVariable(string line, vector<string> tokens) {
    size_t posSpace = 0, posResult = 0, current = 0, posVar = 0, newPos = 0;
    posSpace = line.find(" ");
    posVar = line.find("var");
    newPos = line.find("=");
    // if it is a new var
    if (posVar != string::npos) {
        if (newPos != string::npos) {
            line.erase(0, posSpace + 1);
        } else {
            tokens.push_back(line.substr(0, posSpace));
            line.erase(0, posSpace + 1);
        }
    }
    // separate by spaces or by the equal sign
    posSpace = line.find(' ');
    posResult = line.find('=');
    if (posSpace != string::npos) {
        while (posSpace < posResult) {
            tokens.push_back(line.substr(0, posSpace));
            line.erase(0, posSpace + 1);
            posSpace = line.find(' ');
            current = line.find('=');
            if (current == string::npos) {
                tokens.push_back(line);
                return tokens;
            }
        }
        while (posSpace > posResult) {
            tokens.push_back(line.substr(0, posResult));
            line.erase(0, posResult + 1);
            tokens.push_back("=");
            posSpace = line.find(" ");
            if (posSpace == 0) {
                line.erase(0, 1);
            }
            tokens.push_back(line);
            return tokens;
        }
        // if there aren't spaces in the string
    } else {
        tokens.push_back(line.substr(0, posResult));
        tokens.push_back("=");
        line.erase(0, posResult + 1);
        tokens.push_back(line);
        return tokens;
    }
    return tokens;
}

/*
splits the lines in the file to tokens and returns the result
*/
vector<string> Initializer::lexer(const char *fileName) {
    ifstream fptr;
    vector<string> strLines, tokens;
    string line;
    fptr.open(fileName);
    // if file wasn't open successfully
    if (!fptr.is_open()) {
        cout << "error opening file" << endl;
    }
    while (getline(fptr, line)) {
        size_t pos = 0;
        //string tokens;
        pos = line.find("\n");
        while (pos) {
            strLines.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
            pos = line.find("\n");
            if (pos == string::npos) {
                break;
            }
        }
    }
    // close file after reading all the lines
    fptr.close();
    string scope;
    // runs on all the lines in the strings vector
    for (unsigned int i = 0; i < strLines.size(); i++) {
        size_t posStart = 0, posEnd = 0, posTab = 0, pos = 0;
        posStart = strLines[i].find("{");
        tokens = split(strLines[i], tokens);
        if (posStart != string::npos) {
            for (unsigned int j = i + 1; j < strLines.size(); j++) {
                posTab = strLines[j].find('\t');
                pos = strLines[j].find(" ");
                // if the line begins with tabs or spaces
                if (posTab == 0 || pos == 0) {
                    strLines[j] = ltrim(strLines[j]);
                }
                // insert the condition and his body into one string in the vector
                posEnd = strLines[j].find("}");
                if (posEnd != string::npos) {
                    tokens.push_back(scope);
                    scope = "";
                    i = j;
                    break;
                }
                // put ';' between the lines in the body of the condition
                scope += strLines[j] + ";";
            }
        }
    }
    return tokens;
}

/*
delete the tabs on the beginning of the string
*/
string Initializer::ltrim(string line) {
    const string &chars = "\t ";
    // delete the tab from the string
    line.erase(0, line.find_first_not_of(chars));
    return line;
}

/*
Insert the string into the parameters vector of the program while
the given string is separate to his command and value
*/
vector<string> Initializer::isFunction(string line, vector<string> tokens) {
    size_t posBrackets1 = 0, posBrackets2 = 0, posComma = 0, pos = 0;
    posBrackets1 = line.find("(");
    pos = line.find(" ");
    // if there are brackets
    if (pos < posBrackets1) {
        tokens.push_back(line.substr(0, pos));
    } else {
        tokens.push_back(line.substr(0, posBrackets1));
    }
    line.erase(0, posBrackets1 + 1);
    posComma = line.find(",");
    // if there is a comma
    if (posComma <= line.length()) {
        if (line.find(" ") == posComma - 1) {
            tokens.push_back(line.substr(0, posComma - 1));
        } else {
            tokens.push_back(line.substr(0, posComma));
        }
        line.erase(0, posComma + 1);
    }
    if (line.find(" ") == 0) {
        line.erase(0, 1);
    }
    posBrackets2 = line.find(")");
    tokens.push_back(line.substr(0, posBrackets2));
    line.erase(0, posBrackets2 + 1);
    return tokens;
}

/*
Insert the string into the parameters vector of the program while
the given string is separate to his name of variable, his direction and his path
*/
vector<string> Initializer::initializeVar(string line, vector<string> tokens) {
    size_t pos = 0, posSign = 0;
    pos = line.find(" ");
    // split by spaces
    while (pos != string::npos) {
        // if it is a string
        posSign = line.find('"');
        if (pos > posSign) {
            posSign = line.find('"');
            tokens.push_back(line.substr(0, posSign - 1));
            line.erase(0, posSign - 1);
            break;
        }
        // insert to the vector of strings
        tokens.push_back(line.substr(0, pos));
        line.erase(0, pos + 1);
        pos = line.find(" ");
    }
    size_t posBrackets1 = 0, posBrackets2 = 0;
    // if there is brackets
    posBrackets1 = line.find("(");
    while (posBrackets1 != string::npos) {
        if (posBrackets1 != 0) {
            tokens.push_back(line.substr(0, posBrackets1));
        }
        line.erase(0, posBrackets1 + 1);
        posBrackets1 = line.find("(");
        posBrackets2 = line.find(")");
        while (posBrackets2 != string::npos) {
            tokens.push_back(line.substr(0, posBrackets2));
            line.erase(0, posBrackets2 + 1);
            posBrackets2 = line.find(")");
        }
    }
    if (line.size() != 0) {
        tokens.push_back(line);
    }
    return tokens;
}

/*
Insert the string into the parameters vector of the program while
the given string is separate to his command and the condition
*/
vector<string> Initializer::isCondition(string line, vector<string> tokens) {
    size_t posSpace = 0, posBrackets1 = 0, posBrackets2 = 0;
    posSpace = line.find(" ");
    posBrackets1 = line.find("(");
    // if the condition between brackets
    if (posBrackets1 != string::npos) {
        // checks if we have spaces
        if (posBrackets1 < posSpace) {
            tokens.push_back(line.substr(0, posBrackets1));
            line.erase(0, posBrackets1 + 1);
            posBrackets2 = line.find(")");
            tokens.push_back(line.substr(0, posBrackets2));
            line.erase(0, posBrackets2 + 1);
        } else {
            tokens.push_back(line.substr(0, posSpace));
            line.erase(0, posSpace + 1);
            posBrackets1 = line.find("(");
            tokens.push_back(line.substr(0, posBrackets1));
            line.erase(0, posBrackets1 + 1);
            posBrackets2 = line.find(")");
            tokens.push_back(line.substr(0, posBrackets2));
            line.erase(0, posBrackets2 + 1);
        }
    } else {
        tokens.push_back(line.substr(0, posSpace));
        line.erase(0, posSpace + 1);
        tokens.push_back(line.substr(0, line.length() - 1));
        line.erase(0, line.length() - 1);
    }
    line.erase(0, line.length());
    return tokens;
}

/*
split the words of a string by into a vector of strings
*/
vector<string> Initializer::split(string line, vector<string> tokens) {
    size_t posTab = 0, pos = 0, posEndCondition = 0;
    posTab = line.find('\t');
    pos = line.find(" ");
    // if the line begins with tabs or spaces
    if (posTab == 0 || pos == 0) {
        line = ltrim(line);
    }
    // checks which type of command the line is
    if ((line.find("openDataServer") != string::npos) || (line.find("connectControlClient") != string::npos) ||
        (line.find("Sleep") != string::npos) || (line.find("Print") != string::npos)) {
        tokens = isFunction(line, tokens);
    } else if ((line.find("->") != string::npos) || (line.find("<-") != string::npos)) {
        tokens = initializeVar(line, tokens);
    } else if ((line.find("while") != string::npos) || (line.find("if") != string::npos)) {
        tokens = isCondition(line, tokens);
    } else {
        posEndCondition = line.find("}");
        if (posEndCondition != string::npos) {
            tokens.push_back(line.substr(0, posEndCondition + 1));
            line.erase(0, posEndCondition + 1);
            return tokens;
        }
        tokens = isVariable(line, tokens);
    }
    return tokens;
}

/*
get the parameters of the string by given skip condition
*/
string Initializer::getParams(int current, int skip, string delim, vector<string> params) {
    string str;
    for (int i = current; i < skip + current - 1; i++) {
        string s = params.at(i);
        str += s;
        str += delim;
    }
    // returns the string separate to his parameters by the delimeter
    return str;
}

/*
In charge of receiving the tokens of the current line, parsing and executing
while using the correct command
*/
void Initializer::parser(vector<string> arr) {
    auto *commands = new Commands();
    int index = 0;
    int length = arr.size();
    string params;
    // runs on all the strings in the given vector
    while (index < length) {
        // the current string in the vector
        string current = Utils::strip(arr.at(index));
        Command *c = nullptr;
        // initialize the string to be the correct command
        if (arr.at(index + 1) == "=") {
            c = commands->getCommand("=");
            params = getParams(index, c->getSkip() + 1, ",", arr);
        } else {
            c = commands->getCommand(current);
            params = getParams(index + 1, c->getSkip(), ",", arr);
        }
        // update the index
        if (c != nullptr) {
            index += c->execute(params);
            //this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    } // end of while loop
}

/*
Update the value in the variables map if exist, otherwise initialize the new variable in the map
*/
void Initializer::updateValue(string key, float val) {
    // wait until the variable will be updated
    valLock.lock();

    auto it = Initializer::variableValue.find(key);
    if (it == Initializer::variableValue.end()) {
        Initializer::variableValue.insert(pair<string, float>(key, val));
    } else {
        // if the variable exist in the map
        Initializer::variableValue.at(key) = val;
    }
    // unlock
    valLock.unlock();
}

/*
Update the direction in the directions map if exist, otherwise initialize the new variable with his direction
*/
void Initializer::updateDirection(string key, bool direction) {
    // wait until the direction will be updated
    directLock.lock();

    auto it = Initializer::variableDirection.find(key);
    if (it == Initializer::variableDirection.end()) {
        Initializer::variableDirection.insert(pair<string, bool>(key, direction));
    } else {
        // if the variable exist in the map
        Initializer::variableDirection.at(key) = direction;
    }
    // unlock
    directLock.unlock();
}

/*
Update the path of the variable in the map if exists, otherwise initialize the new variable with his path
*/
void Initializer::updatePath(string key, string path) {
    // wait until the path will be updated
    pathLock.lock();
    auto it = Initializer::variablePath.find(key);
    if (it == Initializer::variablePath.end()) {
        Initializer::variablePath.insert(pair<string, string>(key, path));
    } else {
        // if the variable exist in the map
        Initializer::variablePath.at(key) = path;
    }
    // unlock
    pathLock.unlock();
}

/*
Get the direction by the given name of variable
*/
bool Initializer::getDirection(string key) {
    auto it = variableDirection.find(key);
    if (it == variableDirection.end()) {
        return -1;
    }
    // returns the direction
    return it->second;
}

/*
Get the value by the given name of variable
*/
float Initializer::getVal(string key) {
    auto it = variableValue.find(key);
    if (it == variableValue.end()) {
        return -1;
    }
    // returns the value
    return it->second;
}

/*
Get the path by the given name of variable
*/
string Initializer::getPath(string key) {
    auto it = variablePath.find(key);
    if (it == variablePath.end()) {
        return "";
    }
    // returns the path
    return it->second;
}