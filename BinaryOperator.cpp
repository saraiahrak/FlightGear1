//
// Created by dekel on 09/11/2019.
//

#include "BinaryOperator.h"

using namespace std;

/*
Plus constructor.
*/
Plus::Plus(Expression *l, Expression *r) {
    this->left = l;
    this->right = r;
}

/*
Calculates expression with plus.
*/
float Plus::calculate() {
    return this->left->calculate() + this->right->calculate();
}

/*
Minus constructor.
*/
Minus::Minus(Expression *l, Expression *r) {
    this->left = l;
    this->right = r;
}

/*
Calculates expression with minus.
*/
float Minus::calculate() {
    return this->left->calculate() - this->right->calculate();
}

/*
Multiplication constructor.
*/
Mul::Mul(Expression *l, Expression *r) {
    this->left = l;
    this->right = r;
}

/*
Calculates expression with multiplication.
*/
float Mul::calculate() {
    return this->left->calculate() * this->right->calculate();
}

/*
Division constructor.
*/
Div::Div(Expression *l, Expression *r) {
    this->left = l;
    this->right = r;
}

// Check that not divide by zero
float Div::division(float l, float r) {
    if (r == 0) {
        throw "division by zero";
    }
    return l / r;
}

/*
Calculates expression with division.
*/
float Div::calculate() {
    return division(this->left->calculate(), this->right->calculate());
}