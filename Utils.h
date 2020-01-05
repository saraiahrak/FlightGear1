//
// Created by sarai on 25/12/2019.
//

#ifndef EX3_UTILS_H
#define EX3_UTILS_H

#include <string>
#include <vector>

using namespace std;
/*
Utils class
*/
class Utils {
public:

    static string lstrip(string s);

    static string rstrip(string s);

    static string strip(string s);

    static vector<string> splitByDelim(string toSplit, const char *delim);

    static string trimApostrophe(string& s);

};

#endif //EX3_UTILS_H