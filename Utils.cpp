//
// Created by sarai on 25/12/2019.
//

#include "Utils.h"
#include <string>
#include <cstring>

using namespace std;

/*
delete the spaces on the beginning of the string
*/
string Utils::lstrip(string s) {
    while (s.at(0) == ' ') {
        s.erase(s.begin());
    }
    return s;
}

/*
delete the spaces on the end of the string
*/
string Utils::rstrip(string s) {
    while (s.at(s.length() - 1) == ' ') {
        s.erase(s.end() - 1);
    }

    while (s.at(s.length() - 1) == '\n') {
        s.erase(s.end() - 1);
    }
    return s;
}

/*
returns the string without spaces
*/
string Utils::strip(string s) {
    string trimmed;
    // delete from the beginning and the end of the string
    trimmed = lstrip(s);
    trimmed = rstrip(trimmed);
    return trimmed;
}

/*
split the words of a string by delimeter
*/
vector<string> Utils::splitByDelim(string params, const char *delim) {
    vector<string> v;
    char *ch = nullptr;
    const char *str = params.c_str();
    char *cstr = new char[strlen(str) + 1];
    strcpy(cstr, str);
    ch = strtok(cstr, delim);
    // insert the parameters into a vector of strings
    while (ch != nullptr) {
        string s(ch);
        string s1 = Utils::strip(s);
        v.push_back(s1);
        ch = strtok(nullptr, delim);
    } // end of while loop
    // delete
    delete [] cstr;
    return v;
}

/*
deleted the apostrophe that we added from the string
*/
string Utils::trimApostrophe(string &s) {
    int length = s.length();
    s.erase(0, 1);
    s.erase(length - 2, length - 1);
    return s;
}