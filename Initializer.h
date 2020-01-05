//
// Created by sarai on 20/12/2019.
//

#ifndef EX3_INITIALIZER_H
#define EX3_INITIALIZER_H

#include <string>
#include "iostream"
#include <vector>
#include <map>
#include <mutex>
#include <queue>

using namespace std;


/**
 * Initializer class
 **/
class Initializer {
public:

    static bool stop;

    static mutex valLock;

    static mutex pathLock;

    static mutex directLock;

    static map<string, float> variableValue;

    static map<string, string> variablePath;

    static map<string, bool> variableDirection;

    static vector<string> xmlVals;

    Initializer();

    ~Initializer() = default;

    static void updateValue(string key, float val);

    static void updatePath(string key, string path);

    static void updateDirection(string key, bool direction);

    vector<string> lexer(const char *nameFile);

    vector<string> split(string line, vector<string> tokens);

    string ltrim(string line);

    static void setStop(bool s);

    vector<string> isVariable(string line, vector<string> tokens);

    void parser(vector<string> arr);

    string getParams(int current, int skip, string delim, vector<string> params);

    vector<string> isFunction(string line, vector<string> tokens);

    vector<string> initializeVar(string line, vector<string> tokens);

    vector<string> isCondition(string line, vector<string> tokens);

    static float getVal(string key);

    static string getPath(string key);

    static bool getDirection(string key);

    void initializeXML();
};


#endif //EX3__INTERPRETER_H_