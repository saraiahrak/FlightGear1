//
// Created by sarai on 20/12/2019.
//

#ifndef EX3_COMMAND_H
#define EX3_COMMAND_H

#include <sys/socket.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <cstring>
#include <vector>
#include <map>
#include "Initializer.h"

using namespace std;


/**
 * Command classes
 **/

//General Command Class
class Command {
public:

    //index to skip parameters and move on to next command
    int skip = 0;

    //Execute method for each command type
    virtual int execute(string params) = 0;

    //getter for skip
    void setSkip(int n);

    //setter for skip
    int getSkip();

    //checks if string has apostrophe in the beggining and end
    bool isString(char first, char last);

    //checks if string is a program variable
    bool isVar(string s);

    //checkes if string is a float
    static bool isNum(string s);

    //checks if string is a boolean condition expression
    bool isExp(string s);

    //Destructor
    virtual ~Command() = default;

};


/**
 * Open Server Command
 * Command for creating a local server to communicate with external client
 **/

class OpenSeverCommand : public Command {
public:

    //server socket
    static int serverSocket;

    //set server socket
    static void setServerSocket(int s);

    //Constructor
    OpenSeverCommand();

    //Destructor
    ~OpenSeverCommand() override = default;

    //Execute the command
    int execute(string params) override;

    //Opens the server socket
    static void openServerSocket(int p);

    //reads from external client
    static void readFromClient(int clientSocket);

};

/**
 * Connect Command
 * Command for creating a local client to communicate with external server
 **/

class ConnectCommand : public Command {
public:

    //messages queue to send to server
    static queue<string> messages;

    //local client socket
    static int client_socket;

    //setter for client socket
    static void setClientSocket(int s);

    //Destructor
    ~ConnectCommand() override = default;

    //Constructor
    ConnectCommand();

    //Execute the command
    int execute(string params) override;

    //Open the client socket given an ip and port
    static void openClientSocket(const char *ip, int port);

    //Send messages from queue to external server
    static void sendToSimulator();

    //add a message to messages queue
    static void addToMessages(string msg);

};


/**
 * Define Var Command
 * Command for defining a new variable in program
 **/

class DefineVarCommand : public Command {
public:
    //Constructor
    DefineVarCommand();

    //Destructor
    ~DefineVarCommand() override = default;

    //Execute command
    int execute(string params) override;
};


/**
 * Condition parser Command
 * Command that handles and executes the condition commands
 **/

class ConditionParser : public Command {
public:
    string myCondition;

    string leftCondition = "";

    string rightCondition = "";

    bool parsed = false;

    //setter for parsed flag
    void setParsed(bool isParsed);

    //setter for left expression of condition
    void setRightCondition(string right);

    //setter for right expression of condition
    void setLeftCondition(string left);

    //check if the condition is true or false
    bool checkCondition();

    //setter for current condition
    void setCondition(string c);

    //parse the condition the first time
    void parseCondition();

    //find the delimiter of the boolean expression
    int findDelim();

    //read the condition and determine true/false
    bool readCondition();

    //clear all parameters
    void clearAll();

};


/**
 * If condition command
 * Command that handles and executes the if commands
 * inherites from Condition Parser
 **/

class IfCommand : public ConditionParser {
public:

    //execute if command
    int execute(string params) override;

    //Constructor
    IfCommand();

    //Destructor
    ~IfCommand() override = default;
};


/**
 * While condition command
 * Command that handles and executes the while commands
 * inherites from Condition Parser
 **/

class LoopCommand : public ConditionParser {
public:
    //Constructor
    LoopCommand();

    //Destructor
    ~LoopCommand() override = default;

    //execute while command
    int execute(string params) override;
};


/**
 * Assign command
 * Handles assignments of new values to program variables
 **/

class AssignCommand : public Command {
public:

    //Constructor
    AssignCommand();

    //Destructor
    ~AssignCommand() override = default;

    //execute the command
    int execute(string params) override;

};


/**
 * Print command
 * Executes printing of given expression
 **/

class PrintCommand : public Command {
public:
    //Constructor
    PrintCommand();

    //Destructor
    ~PrintCommand() override = default;

    //Execute the command
    int execute(string params) override;
};

/**
 * Sleep command
 * Handles sleep of threads
 **/

class SleepCommand : public Command {
public:
    //Constructor
    SleepCommand();

    //Destructor
    ~SleepCommand() override = default;

    //Execute the command
    int execute(string params) override;
};


#endif //EX3_COMMAND_H