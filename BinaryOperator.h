//
// Created by sarai on 23/12/2019.
//

#ifndef EX3_BINARYOPERATOR_H

#include "Expression.h"

/*
Binary expression.
*/
class BinaryOperator : public Expression {
protected:

    //Components of Binary Expression
    Expression *left = nullptr;
    Expression *right = nullptr;
public:

    ~BinaryOperator() override = default;
};

/*
Plus binary expression
*/
class Plus : public BinaryOperator {
public:

    Plus(Expression *left, Expression *right);

    float calculate() override;
};

/*
Minus binary expression
*/
class Minus : public BinaryOperator {
public:
    Minus(Expression *left, Expression *right);

    float calculate() override;
};

/*
Multiplication binary expression
*/
class Mul : public BinaryOperator {
public:
    Mul(Expression *left, Expression *right);

    float calculate() override;
};

/*
Division binary expression
*/
class Div : public BinaryOperator {
public:
    Div(Expression *left, Expression *right);

    float calculate() override;

    float division(float l, float r);
};

#endif //EX3_BINARYOPERATOR_H