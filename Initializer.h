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

    //flag for stop program
    static bool stop;

    //mutex locks for maps- different threads use maps in parallel
    static mutex valLock;

    static mutex pathLock;

    static mutex directLock;

    //<Key: Value> = <VariableName: Value> map
    static map<string, float> variableValue;

    //<Key: Value> = <VariableName: Path> map
    static map<string, string> variablePath;

    //<Key: Value> = <VariableName: Update-Direction> map
    static map<string, bool> variableDirection;

    //XML paths for variables in external simulator
    static vector<string> xmlVals;

    //Constructor
    Initializer();

    //Destructor
    ~Initializer() = default;

    //Update the path in variablePath map
    static void updatePath(string key, string path);

    //Update the value in variableValue map
    static void updateValue(string key, float val);

    //Update the direction in variableDirection map
    static void updateDirection(string key, bool direction);

    //method that splits the commands from the text file to vector
    vector<string> lexer(const char *nameFile);

    //split lines
    vector<string> split(string line, vector<string> tokens);

    //trim left tabs and spaces from string
    string ltrim(string line);

    //set the stop flag
    static void setStop(bool s);

    vector<string> isVariable(string line, vector<string> tokens);

    //parse the vector of commands and execute commands accordingly
    void parser(vector<string> arr);

    //Join the parameters according to the command skip index
    string getParams(int current, int skip, string delim, vector<string> params);

    vector<string> isFunction(string line, vector<string> tokens);

    vector<string> initializeVar(string line, vector<string> tokens);

    vector<string> isCondition(string line, vector<string> tokens);

    //the value in value map
    static float getVal(string key);

    //get path in path map
    static string getPath(string key);

    //get direction in derection map
    static bool getDirection(string key);

    //initialize the xml vector
    void initializeXML();
};


#endif //EX3__INTERPRETER_H_