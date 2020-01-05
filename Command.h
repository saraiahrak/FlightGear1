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

class Command {
public:

    int skip = 0;

    virtual int execute(string params) = 0;

    int getSkip();

    void setSkip(int n);

    bool isString(char first, char last);

    bool isVar(string s);

    static bool isNum(string s);

    bool isExp(string s);

    virtual ~Command() = default;


};

class OpenSeverCommand : public Command {
public:

    static int serverSocket;

    static void setServerSocket(int s);

    OpenSeverCommand();

    ~OpenSeverCommand() override = default;

    int execute(string params) override;

    static void openServerSocket(int p);

    static void readFromClient(int clientSocket);

};

class ConnectCommand : public Command {
public:

    static queue<string> messages;

    static int client_socket;

    static void setClientSocket(int s);

    ConnectCommand();

    ~ConnectCommand() override = default;

    int execute(string params) override;

    static void openClientSocket(const char* ip, int port);

    static void sendToSimulator();

    static void addToMessages(string msg);

};

class DefineVarCommand : public Command {
public:
    DefineVarCommand();

    ~DefineVarCommand() override = default;

    int execute(string params) override;
};

class ConditionParser : public Command {
public:
    string myCondition;

    string leftCondition = "";

    string rightCondition = "";

    bool parsed = false;

    void setParsed(bool isParsed);

    void setLeftCondition(string left);

    void setRightCondition(string right);

    bool checkCondition();

    void setCondition(string c);

    void parseCondition();

    int findDelim();

    bool readCondition();

    void clearAll();

};

class IfCommand : public ConditionParser {
public:

    int execute(string params) override;

    IfCommand();

    ~IfCommand() override = default;
};


class LoopCommand : public ConditionParser {
public:
    LoopCommand();

    ~LoopCommand() = default;

    int execute(string params) override;
};

class AssignCommand : public Command {
public:

    AssignCommand();

    ~AssignCommand() = default;

    int execute(string params) override;

};

class PrintCommand : public Command {
public:
    PrintCommand();

    ~PrintCommand() = default;

    int execute(string params) override;
};

class SleepCommand : public Command {
public:
    SleepCommand();

    ~SleepCommand() = default;

    int execute(string params) override;

};


#endif //EX3_COMMAND_H