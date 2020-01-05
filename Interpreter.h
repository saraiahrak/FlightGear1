//
// Created by dekel on 10/11/2019.
//
#ifndef EX3_INTERPRETER_H
#define EX3_INTERPRETER_H

#include <string>
#include <map>
#include <vector>
#include "Expression.h"
#include "Variable.h"
#include "BinaryOperator.h"
#include "UnaryOperator.h"
#include "Initializer.h"

using namespace std;

class Interpreter {
private:
    //map<string, double> variablesList = {};
    string infix;
    Expression *exp = nullptr;
public:
    virtual ~Interpreter() {}

    Interpreter();

    void wrongInput(string str);

    void wrongVar(string args);

    void evaluateVars();

    void setVariables(string input);

    void setVariables();

    Expression *interpret(string str);

    Expression *interpret();

    void divide();

    bool isDigit(char c);

    void negativeUnaryExp();

    vector<string> splitByDelimeter(string str, string delimeter);

    bool isNumber(string token);

    string convertToPostfix();

    bool isOperator(string token);

    Expression *fromPostfixToExpr(string postfix);

    int operationSign(string operation);
};


#endif //EX3_INTERPRETER_H