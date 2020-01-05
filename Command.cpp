//
// Created by sarai on 20/12/2019.
//
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <vector>
#include <fstream>
#include "Command.h"
#include "Initializer.h"
#include "Interpreter.h"
#include "Utils.h"
#include <thread>

using namespace std;

//set skip to command
void Command::setSkip(int n) {
    this->skip = n;
}

int Command::getSkip() {
    return this->skip;
}

bool Command::isExp(string s) {
    auto it = 0, i = 0;
    array<const char *, 6> delims{"<", ">", "<=", ">=", "==", "!="};

    while (i < delims.size()) {
        it = s.find(delims.at(i));
        if (it != string::npos) {
            return true;
        }
        i++;
    }
    return false;
}

bool Command::isNum(string s) {
    int i = 0;
    while (i < s.length()) {
        if (i == 0 && (s.at(i) == '-' || s.at(i) == '+')) {
            i++;
            continue;
        }
        if ((s.at(i) < '0' || s.at(i) > '9') && s.at(i) != '.') {
            return false;
        }
        i++;
    }
    return true;
}

bool Command::isVar(string s) {
    if (Initializer::variableValue.find(s) != Initializer::variableValue.end()) {
        return true;
    }
    return !isNum(s) && !isExp(s);

}

int OpenSeverCommand::serverSocket;

void OpenSeverCommand::setServerSocket(int s) {
    OpenSeverCommand::serverSocket = s;
}

OpenSeverCommand::OpenSeverCommand() {
    this->setSkip(2);
}

bool Command::isString(char first, char last) {
    if (first == '"') {
        if (last == '"') {
            return true;
        }
    }

    return false;
}

void OpenSeverCommand::readFromClient(int clientSocket) {
    vector<float> values;
    string current = "";
    string valsTest = "";
    while (!Initializer::stop) {
        if (!values.empty()) {
            values.clear();
            current = "";
        }
        char buffer[1] = {0};
        int valread = read(clientSocket, buffer, 1);
        if (valread < 0) {
            continue;
        }
        while (buffer[0] != '\n') {
            if (buffer[0] == ',') {
                values.push_back(stof(current));
                current = "";
                read(clientSocket, buffer, 1);
                continue;
            }
            current += buffer[0];
            valread = read(clientSocket, buffer, 1);
        }
        if (!current.empty()) {
            values.push_back(stof(current));
        }

        for (int i = 0; i < values.size(); i++) {
            string path = Initializer::xmlVals.at(i);
            float val = values.at(i);
            string var = "";

            auto it = Initializer::variablePath.begin();
            // Iterate through the map
            while (it != Initializer::variablePath.end()) {
                // Check if value of this entry matches with given value
                if (it->second == path) {
                    var = it->first;
                    break;
                }
                // Go to next entry in map
                it++;
            }

            bool direction = Initializer::getDirection(var);

            if (!direction) {
                Initializer::updateValue(var, val);
            }
        }
    }
}

void OpenSeverCommand::openServerSocket(int p) {
    //create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        //error
        cerr << "Could not create a socket" << endl;
    }
    setServerSocket(socketfd);
    //bind socket to IP address
    sockaddr_in address; //in means IP4
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //give me any IP allocated for my machine
    address.sin_port = htons(p);

    if (bind(socketfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
        cerr << "Could not bind the socket to an IP" << endl;
    }

    //listen to port
    if (listen(socketfd, 5) == -1) {
        cerr << "Error during listening command" << endl;
    } else {
        cout << "Server is now listening ..." << endl;
    }
    // accepting a client
    int client_socket = accept(socketfd, (struct sockaddr *) &address,
                               (socklen_t *) &address);

    while (client_socket == -1) {
        cerr << "Error accepting client, trying again" << endl;
        client_socket = accept(socketfd, (struct sockaddr *) &address,
                               (socklen_t *) &address);
    }

    close(socketfd); //closing the listening socket

    cout << "connected to client" << endl;

    //reading from client
    thread serverThread(readFromClient, client_socket);
    serverThread.detach();
}

int OpenSeverCommand::execute(string params) {
    vector<string> p = Utils::splitByDelim(params, ",");
    int port = stoi(p[0]);
    openServerSocket(port);

    return this->skip;
}

int ConnectCommand::client_socket;

queue<string> ConnectCommand::messages;

void ConnectCommand::addToMessages(string msg) {
    ConnectCommand::messages.push(msg);
}

ConnectCommand::ConnectCommand() {
    this->setSkip(3);
}

int ConnectCommand::execute(string params) {
    auto *i = new Interpreter();
    vector<string> p = Utils::splitByDelim(params, ",");
    string ip = Utils::trimApostrophe(p.at(0));
    const char *myIp = ip.c_str();
    int port = (int) (i->interpret(p[1])->calculate());
    openClientSocket(myIp, port);
    return this->skip;
}

void ConnectCommand::setClientSocket(int s) {
    ConnectCommand::client_socket = s;
}

void ConnectCommand::openClientSocket(const char *ip, int port) {
    //create socket
    int cSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (cSocket == -1) {
        //error
        std::cerr << "Could not create a socket" << std::endl;
    }
    setClientSocket(cSocket);


    //create a sockaddr obj to hold address of server
    sockaddr_in address; //in means IP4
    address.sin_family = AF_INET;//IP4
    address.sin_addr.s_addr = inet_addr(ip);  //the localhost address
    address.sin_port = htons(port);

    int is_connect = connect(cSocket, (struct sockaddr *) &address, sizeof(address));
    if (is_connect == -1) {
        std::cerr << "waiting for host server..." << std::endl;
    }

    std::cout << "Client is now connected to server" << std::endl;

    thread clientThread(sendToSimulator);
    clientThread.detach();
}

void ConnectCommand::sendToSimulator() {
    while (!Initializer::stop) {
        if (ConnectCommand::messages.empty()) {
            continue;
        }
        string toSend = ConnectCommand::messages.front();

        int is_sent = write(ConnectCommand::client_socket, toSend.c_str(), toSend.length());
        if (is_sent == -1) {
            cout << "Error sending message" << endl;
            continue;
        } else {
            ConnectCommand::messages.pop();
            this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
}

DefineVarCommand::DefineVarCommand() {
    this->setSkip(5);
}

int DefineVarCommand::execute(string params) {
    vector<string> p = Utils::splitByDelim(params, ",");
    Initializer::updateValue(p[0], 0.0);
    Initializer::updatePath(p[0], Utils::trimApostrophe(p[3]));
    if (p[1] == "<-") {
        Initializer::updateDirection(p[0], false);
    } else if (p[1] == "->") {
        Initializer::updateDirection(p[0], true);
    }
    return this->skip;
}

IfCommand::IfCommand() {
    this->setSkip(3);
}

AssignCommand::AssignCommand() {
    this->setSkip(3);
}

int AssignCommand::execute(string params) {
    auto *i = new Interpreter();
    Expression *e = nullptr;
    vector<string> v = Utils::splitByDelim(params, ",");
    string var = v[0];
    string val = v[2];

    e = i->interpret(val);
    float value = e->calculate();

    Initializer::updateValue(var, value);
    string path = Initializer::getPath(var);

    if (!path.empty() && Initializer::getDirection(var)) {
        string message = "set " + path + " " + to_string(value) + "\r\n";
        ConnectCommand::addToMessages(message);
    }
    return this->skip;
}

SleepCommand::SleepCommand() {
    this->setSkip(2);
}

int SleepCommand::execute(string params) {
    Interpreter *i = new Interpreter();
    Expression *e = nullptr;

    vector<string> v = Utils::splitByDelim(params, ",");

    e = i->interpret(v[0]);
    this_thread::sleep_for(std::chrono::milliseconds((int) e->calculate()));
    return this->skip;
}

PrintCommand::PrintCommand() {
    this->setSkip(2);
}

int PrintCommand::execute(string params) {
    vector<string> v = Utils::splitByDelim(params, ",");
    string toPrint = v[0];
    if (isString(toPrint.at(0), toPrint.at(toPrint.length() - 1))) {
        toPrint = Utils::trimApostrophe(toPrint);
        cout << toPrint << endl;
        return this->skip;
    }

    Expression *e = nullptr;
    auto *i = new Interpreter();

    e = i->interpret(toPrint);
    cout << e->calculate() << endl;

    if (toPrint == "done") {
        Initializer::setStop(true);
    }

    return this->skip;
}

LoopCommand::LoopCommand() {
    this->setSkip(3);
}

int IfCommand::execute(string params) {

    const char *filename = "IfCommands.txt";
    ofstream outFile{filename, ios::trunc};
    vector<string> p = Utils::splitByDelim(params, ",");

    auto *i = new Initializer();
    vector<string> v = Utils::splitByDelim(p[1], ";");
    this->setCondition(p[0]);

    if (checkCondition()) {
        if (!outFile.is_open()) {
            cerr << "Couldn't open file" << endl;
        }
        for (string s : v) {
            outFile << s;
            outFile << '\n';
        }
        outFile.close();

        vector<string> myCommands = i->lexer(filename);
        i->parser(myCommands);
    }

    clearAll();
    return this->getSkip();
}


int LoopCommand::execute(string params) {
    const char *filename = "LoopCommands.txt";
    ofstream outFile{filename, ios::trunc};
    vector<string> p = Utils::splitByDelim(params, ",");

    auto *i = new Initializer();
    vector<string> v = Utils::splitByDelim(p[1], ";");
    this->setCondition(p[0]);

    if (checkCondition()) {
        if (!outFile.is_open()) {
            cerr << "Couldn't open file" << endl;
        }
        for (string s : v) {
            outFile << s;
            outFile << '\n';
        }
        outFile.close();
    }

    while (checkCondition()) {
        vector<string> myCommands = i->lexer(filename);
        i->parser(myCommands);
    }

    clearAll();
    return this->getSkip();
}

void ConditionParser::clearAll() {
    leftCondition.clear();
    rightCondition.clear();
    myCondition.clear();
    setParsed(false);
}

void ConditionParser::setParsed(bool isParsed) {
    this->parsed = isParsed;
}

void ConditionParser::setLeftCondition(string left) {
    this->leftCondition = left;
}

void ConditionParser::setRightCondition(string right) {
    this->rightCondition = right;
}

bool ConditionParser::checkCondition() {
    if (isVar(myCondition)) {
        float val = Initializer::getVal(myCondition);
        return val;
    } else if (isExp(myCondition)) {
        if (!parsed) {
            parseCondition();
        }
        return readCondition();
    } else {
        auto *i = new Interpreter();
        Expression *e = nullptr;
        e = i->interpret(myCondition);
        return e->calculate();
    }
}

void ConditionParser::setCondition(string c) {
    this->myCondition = c;
}

void ConditionParser::parseCondition() {
    array<const char *, 6> delims = {"<", ">", "<=", ">=", "==", "!="};
    int pos = findDelim();

    const char *delim = delims.at(pos);
    string tempCondition(myCondition);
    vector<string> v = Utils::splitByDelim(tempCondition, delim);
    setLeftCondition(v.at(0));
    setRightCondition(v.at(1));
    setParsed(true);
}

int ConditionParser::findDelim() {
    enum delims {
        small, big, smallEqual, bigEqual, equal, different
    };

    size_t foundSmall = myCondition.find('<');
    size_t foundBig = myCondition.find('>');
    size_t foundSmallEqual = myCondition.find("<=");
    size_t foundBigEqual = myCondition.find(">=");
    size_t foundEqual = myCondition.find("==");
    size_t foundDifferent = myCondition.find("!=");

    if (foundSmall != string::npos && foundSmallEqual == string::npos) {
        return small;
    }
    if (foundBig != string::npos && foundBigEqual == string::npos) {
        return big;
    }
    if (foundSmallEqual != string::npos) {
        return smallEqual;
    }
    if (foundBigEqual != string::npos) {
        return bigEqual;
    }
    if (foundEqual != string::npos) {
        return equal;
    }
    if (foundDifferent != string::npos) {
        return different;
    }
}

bool ConditionParser::readCondition() {

    array<const char *, 6> delims = {"<", ">", "<=", ">=", "==", "!="};
    int pos = findDelim();

    auto *i = new Interpreter();
    Expression *left = nullptr;
    Expression *right = nullptr;
    left = i->interpret(string(leftCondition));
    right = i->interpret(string(rightCondition));

    float l = left->calculate();
    float r = right->calculate();

    switch (pos) {
        case 0:
            return l < r;
        case 1:
            return l > r;
        case 2:
            return l <= r;
        case 3:
            return l >= r;
        case 4:
            return l == r;
        case 5:
            return l != r;
        default:
            return false;
    }
}
