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

//set skip index to command
void Command::setSkip(int n) {
    this->skip = n;
}

//get skip index to command
int Command::getSkip() {
    return this->skip;
}

//checks if a string is a condition expression
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

//checks if a string is a number in float format
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


//checks if a string is a variable name in the variable map
bool Command::isVar(string s) {
    if (Initializer::variableValue.find(s) != Initializer::variableValue.end()) {
        return true;
    }
    return !isNum(s) && !isExp(s);

}

int OpenSeverCommand::serverSocket;

//set the server socket after opened
void OpenSeverCommand::setServerSocket(int s) {
    OpenSeverCommand::serverSocket = s;
}

//Constructor for OpenServerCommand
OpenSeverCommand::OpenSeverCommand() {

    //set the skip uppon creation
    this->setSkip(2);
}

//checks if a string is a string that starts and ends with apostrophe
bool Command::isString(char first, char last) {
    if (first == '"') {
        if (last == '"') {
            return true;
        }
    }
    return false;
}

//reads the information sent from the server
void OpenSeverCommand::readFromClient(int clientSocket) {
    vector<float> values;
    string current = "";
    string valsTest = "";
    while (!Initializer::stop) {
        //if values is not empty in the beggining of the iteration- clear to receive new values
        if (!values.empty()) {
            values.clear();
            current = "";
        }


        char buffer[1] = {0};
        int valread = read(clientSocket, buffer, 1);
        if (valread < 0) {
            continue;
        }

        //reads information that ends with new line, each value seperated by comma
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
        //insert last value
        if (!current.empty()) {
            values.push_back(stof(current));
        }

        //iterate over received values, find the variable and direction
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

            //if the direction is false- update value in our map
            if (!direction) {
                Initializer::updateValue(var, val);
            }
        }
    }
}


//Open the server socket
void OpenSeverCommand::openServerSocket(int p) {
    //create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        //error creating socket
        cerr << "Could not create a socket" << endl;
    }
    setServerSocket(socketfd);

    //bind socket to IP address
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(p);

    if (bind(socketfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
        //error binding
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

    //reading from client in separate thread
    thread serverThread(readFromClient, client_socket);
    serverThread.detach();
}


//Execute the open server command
int OpenSeverCommand::execute(string params) {
    auto *i = new Interpreter();
    vector<string> p = Utils::splitByDelim(params, ",");

    //calculating the listening port
    int port = (int) (i->interpret(p[0])->calculate());
    openServerSocket(port);

    return this->skip;
}

int ConnectCommand::client_socket;


queue<string> ConnectCommand::messages;

//Add a message to the queue- to send to server
void ConnectCommand::addToMessages(string msg) {
    ConnectCommand::messages.push(msg);
}

//Constructor for Connect command
ConnectCommand::ConnectCommand() {
    //set skip uppon creation
    this->setSkip(3);
}

//Execute the Connect Command
int ConnectCommand::execute(string params) {
    auto *i = new Interpreter();
    //take out ip and port from parameters
    vector<string> p = Utils::splitByDelim(params, ",");
    string ip = Utils::trimApostrophe(p.at(0));
    const char *myIp = ip.c_str();
    int port = (int) (i->interpret(p[1])->calculate());

    //open the client socket
    openClientSocket(myIp, port);

    //return the skip index for next command in line
    return this->skip;
}

//Set the client socket after creation
void ConnectCommand::setClientSocket(int s) {
    ConnectCommand::client_socket = s;
}

//Create the client socket and thread that sends messages to server
void ConnectCommand::openClientSocket(const char *ip, int port) {
    //create socket
    int cSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (cSocket == -1) {
        //error creating socket
        std::cerr << "Could not create a socket" << std::endl;
    }
    setClientSocket(cSocket);


    //create a sockaddr obj to hold address of server
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);

    //Connecting to server
    int is_connect = connect(cSocket, (struct sockaddr *) &address, sizeof(address));
    if (is_connect == -1) {
        //error connecting to server
        std::cerr << "waiting for host server..." << std::endl;
    }

    std::cout << "Client is now connected to server" << std::endl;

    //Create thread that sends messages to simulator
    thread clientThread(sendToSimulator);
    clientThread.detach();
}

//Send the external server messages throught the program
void ConnectCommand::sendToSimulator() {
    while (!Initializer::stop) {
        //checks if there are messages awaiting send
        if (ConnectCommand::messages.empty()) {
            continue;
        }

        //first message in queue
        string toSend = ConnectCommand::messages.front();

        //sending message to external server
        int is_sent = write(ConnectCommand::client_socket, toSend.c_str(), toSend.length());
        if (is_sent == -1) {
            //Error sending message- trying again
            cout << "Error sending message" << endl;
            continue;
        } else {
            //pop handled message to continue to next message
            ConnectCommand::messages.pop();
            this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
}

//Define Var Command constructor
DefineVarCommand::DefineVarCommand() {

    //set skip index uppon creation
    this->setSkip(5);
}

//Execute the Define Var Command
int DefineVarCommand::execute(string params) {
    //Extract parameters from string
    vector<string> p = Utils::splitByDelim(params, ",");

    //New defined variable- updating path, value and direction maps
    Initializer::updateValue(p[0], 0.0);
    Initializer::updatePath(p[0], Utils::trimApostrophe(p[3]));
    if (p[1] == "<-") {
        Initializer::updateDirection(p[0], false);
    } else if (p[1] == "->") {
        Initializer::updateDirection(p[0], true);
    }

    //return the skip index for next command
    return this->skip;
}


//Assign Command Constructor
AssignCommand::AssignCommand() {

    //set skip index uppon creation
    this->setSkip(3);
}

//Execute the Assign Command
int AssignCommand::execute(string params) {
    auto *i = new Interpreter();
    Expression *e = nullptr;
    //extracting the parameters
    vector<string> v = Utils::splitByDelim(params, ",");
    string var = v[0];
    string val = v[2];

    //calculating value
    e = i->interpret(val);
    float value = e->calculate();

    //updating the value
    Initializer::updateValue(var, value);
    string path = Initializer::getPath(var);

    //if direction is true-message should be sent to external server
    if (!path.empty() && Initializer::getDirection(var)) {
        string message = "set " + path + " " + to_string(value) + "\r\n";
        // add the message to the message queue
        ConnectCommand::addToMessages(message);
    }

    //return the skip index for next  command
    return this->skip;
}

//Sleep Command Constructor
SleepCommand::SleepCommand() {

    //set the skip index uppon creation
    this->setSkip(2);
}

//Execute the Sleep Command
int SleepCommand::execute(string params) {
    Interpreter *i = new Interpreter();
    Expression *e = nullptr;

    //extracting parameters
    vector<string> v = Utils::splitByDelim(params, ",");

    //calculating sleep time
    e = i->interpret(v[0]);

    //this thread sleeps for the given amount of time
    this_thread::sleep_for(std::chrono::milliseconds((int) e->calculate()));

    //return the skip index for next command
    return this->skip;
}


//Print Command Constructor
PrintCommand::PrintCommand() {

    //set skip index uppon creation
    this->setSkip(2);
}

//Execute Print Command
int PrintCommand::execute(string params) {
    //extract parameters
    vector<string> v = Utils::splitByDelim(params, ",");

    string toPrint = v[0];

    //if it's a string- trim the apostrophe and print the string
    if (isString(toPrint.at(0), toPrint.at(toPrint.length() - 1))) {
        toPrint = Utils::trimApostrophe(toPrint);
        cout << toPrint << endl;
        //return the skip index for next command
        return this->skip;
    }

    //Otherwise- message is a number
    Expression *e = nullptr;
    auto *i = new Interpreter();

    //calculate the number and print
    e = i->interpret(toPrint);
    cout << e->calculate() << endl;

    //set stop to program
    if (toPrint == "done") {
        Initializer::setStop(true);
    }

    //return the skip index for next command
    return this->skip;
}

//Constructor for While Loop Command
LoopCommand::LoopCommand() {

    //set skip index uppon creation
    this->setSkip(3);
}


//If Command Constructor
IfCommand::IfCommand() {

    //set skip index uppon creation
    this->setSkip(3);
}


//Execute If Command
int IfCommand::execute(string params) {

    //Create If Commands file that contains all command to be executed
    const char *filename = "IfCommands.txt";
    ofstream outFile{filename, ios::trunc};

    //extract parameters- condition and commands to execute
    vector<string> p = Utils::splitByDelim(params, ",");
    this->setCondition(p[0]);
    vector<string> v = Utils::splitByDelim(p[1], ";");

    auto *i = new Initializer();

    //Check if condition is true
    if (checkCondition()) {
        if (!outFile.is_open()) {
            cerr << "Couldn't open file" << endl;
        }
        //print commands to file
        for (string s : v) {
            outFile << s;
            outFile << '\n';
        }
        outFile.close();

        //execute commands with lexer and parser
        vector<string> myCommands = i->lexer(filename);
        i->parser(myCommands);
    }

    //clear parameters
    clearAll();

    //return skip index for next command
    return this->getSkip();
}

//Execute While Loop command
int LoopCommand::execute(string params) {

    //Create While Commands file that contains all command to be executed
    const char *filename = "LoopCommands.txt";
    ofstream outFile{filename, ios::trunc};

    //extract parameters- condition and commands to execute
    vector<string> p = Utils::splitByDelim(params, ",");
    this->setCondition(p[0]);
    vector<string> v = Utils::splitByDelim(p[1], ";");

    auto *i = new Initializer();

    //Check if condition is true
    if (checkCondition()) {
        if (!outFile.is_open()) {
            cerr << "Couldn't open file" << endl;
        }
        //print commands to file
        for (string s : v) {
            outFile << s;
            outFile << '\n';
        }
        outFile.close();
    }

    //While condition is true- use lexer and parser to execute the command
    while (checkCondition()) {
        vector<string> myCommands = i->lexer(filename);
        i->parser(myCommands);
    }

    //clear parameters
    clearAll();

    //return the skip index for next command
    return this->getSkip();
}

//Clear all parameters for if and while loop commands
void ConditionParser::clearAll() {
    leftCondition.clear();
    rightCondition.clear();
    myCondition.clear();
    setParsed(false);
}

//Set 'parsed' flag for the condition command
void ConditionParser::setParsed(bool isParsed) {
    this->parsed = isParsed;
}

//set the left expression string for the condition
void ConditionParser::setLeftCondition(string left) {
    this->leftCondition = left;
}

//set the right expression string for the condition
void ConditionParser::setRightCondition(string right) {
    this->rightCondition = right;
}

//Check if given condition is true or false
bool ConditionParser::checkCondition() {
    //if it's a variable from map- return the value
    if (isVar(myCondition)) {
        float val = Initializer::getVal(myCondition);
        return val;
        //if it's a boolean expression- parse the condition once and read it
    } else if (isExp(myCondition)) {
        if (!parsed) {
            parseCondition();
        }
        return readCondition();
        //otherwise- calculate the number and return it
    } else {
        auto *i = new Interpreter();
        Expression *e = nullptr;
        e = i->interpret(myCondition);
        return e->calculate();
    }
}

//Set the condition for the condition command
void ConditionParser::setCondition(string c) {
    this->myCondition = c;
}

//parse the condition
void ConditionParser::parseCondition() {
    //find the appropriate delimiter
    array<const char *, 6> delims = {"<", ">", "<=", ">=", "==", "!="};
    int pos = findDelim();

    const char *delim = delims.at(pos);
    string tempCondition(myCondition);

    //extract left and right expressions of the condition
    vector<string> v = Utils::splitByDelim(tempCondition, delim);
    setLeftCondition(v.at(0));
    setRightCondition(v.at(1));
    setParsed(true);
}

//find the delimiter for the given boolean expression
int ConditionParser::findDelim() {

    //representing the possible delimiters by enum
    enum delims {
        small, big, smallEqual, bigEqual, equal, different
    };

    size_t foundSmall = myCondition.find('<');
    size_t foundBig = myCondition.find('>');
    size_t foundSmallEqual = myCondition.find("<=");
    size_t foundBigEqual = myCondition.find(">=");
    size_t foundEqual = myCondition.find("==");
    size_t foundDifferent = myCondition.find("!=");

    //return the appropriate delimiter
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


//Read the condition and return true or false
bool ConditionParser::readCondition() {

    array<const char *, 6> delims = {"<", ">", "<=", ">=", "==", "!="};
    //finding delimiter
    int pos = findDelim();

    auto *i = new Interpreter();
    Expression *left = nullptr;
    Expression *right = nullptr;

    //calculating the left and right expressions of the condition
    left = i->interpret(string(leftCondition));
    right = i->interpret(string(rightCondition));

    float l = left->calculate();
    float r = right->calculate();

    //returning the boolean value according to the delimiter
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
