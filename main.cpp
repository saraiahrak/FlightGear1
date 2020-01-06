//
// Created by sarai on 20/12/2019.
//

#include <vector>
#include "iostream"
#include "Initializer.h"
#include "Command.h"

using namespace std;

int main(int argc, char *argv[]) {

    //Initializer of program
    auto *initializer = new Initializer();
    if (argc == 2) {
        //recieve the commands file name
        const char *fileName = argv[1];

        //create a commands & parameters vector from given file
        vector<string> detailsList = initializer->lexer(fileName);

        //parse and execute commands
        initializer->parser(detailsList);

        //close the program local server and client sockets
        close(OpenSeverCommand::serverSocket);
        close(ConnectCommand::client_socket);
    } else {
        //error- couldn't open file
        cout << "invalid name file!" << endl;
    }


    return 0;
}
