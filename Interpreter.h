//
// Created by dekel on 10/11/2019.
//
#ifndef EX3_INTERPRETER_H
#define EX3_INTERPRETER_H

#include <string>
#include <map>
#include <vector>
#include "Expression.h"
#include "BinaryOperator.h"
#include "UnaryOperator.h"
#include "Initializer.h"

using namespace std;


/**
 * Interpreter class
 * Interprets mathematical expression given as strings
 **/
class Interpreter {
private:
    string infix;
    Expression *exp = nullptr;
public:
    //destructor
    virtual ~Interpreter() = default;

    //costructor
    Interpreter();

    //checks for errors
    void wrongInput(string str);

    void wrongVar(string args);

    void evaluateVars();

    void setVariables(string input);

    void setVariables();

    //interprets a given string and returns expression
    Expression *interpret(string str);

    //Error if a string wasnt given
    Expression *interpret();

    //check for zero division
    void divide();

    //check if a character is a digit
    bool isDigit(char c);

    void negativeUnaryExp();

    //split a string by given delimiter
    vector<string> splitByDelimeter(string str, string delimeter);

    //checks if a string is a number
    bool isNumber(string token);

    //converts the infix string to postfix
    string convertToPostfix();

    //checks if a token is a mathematical operator
    bool isOperator(string token);

    //converts a postfix string to an Expression object
    Expression *fromPostfixToExpr(string postfix);

    int operationSign(string operation);
};


#endif //EX3_INTERPRETER_H