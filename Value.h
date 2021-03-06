//
// Created by sarai on 23/12/2019.
//
#ifndef EX3_VALUE_H
#define EX3_VALUE_H

#include "Expression.h"

/**
 * Value class
 **/

class Value : public Expression {
private:
    float value;
public:
    Value(float value);

    float calculate() override;
};

#endif //EX3_VALUE_H