//
// Created by sarai on 23/12/2019.
//

#ifndef EX3_VARIABLE_H
#define EX3_VARIABLE_H

#include <string>
#include <vector>
#include <map>
#include "Value.h"
#include "Expression.h"

using namespace std;

/*
Binary expression.
*/
class Variable : public Expression {
private:
    string variable;
    double value;
public:
    virtual ~Variable();

    Variable(string var, float val);

    Variable &operator++();

    Variable &operator++(int);

    Variable &operator--();

    Variable &operator--(int);

    Variable &operator+=(float num);

    Variable &operator-=(float num);

    float calculate();
};

#endif //EX3_VARIABLE_H