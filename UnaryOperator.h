//
// Created by sarai on 23/12/2019.
//

#ifndef EX3_UNARYOPERATOR_H
#define EX3_UNARYOPERATOR_H

#include "Expression.h"

/**
 * Unary Operator class
 * Expressions with unary operator
 **/
class UnaryOperator : public Expression {
protected:
    Expression *expression = nullptr;
public:
    virtual ~UnaryOperator() {}
};

/*
Plus unary expression
*/
class UPlus : public UnaryOperator {
public:
    UPlus(Expression *exp);

    float calculate();
};

/*
Minus unary expression
*/
class UMinus : public UnaryOperator {
public:
    UMinus(Expression *exp);

    float calculate();
};

#endif //EX3_UNARYOPERATOR_H