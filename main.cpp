//
// Created by sarai on 20/12/2019.
//

#include <vector>
#include "iostream"
#include "Initializer.h"
#include "Command.h"

using namespace std;

int main(int argc, char *argv[]) {


    auto *initializer = new Initializer();
    if (argc == 2) {
        const char *fileName = argv[1];
        vector<string> detailsList = initializer->lexer(fileName);
        initializer->parser(detailsList);
        close(OpenSeverCommand::serverSocket);
        close(ConnectCommand::client_socket);
    } else {
        cout << "invalid name file!" << endl;
    }


    return 0;
}
