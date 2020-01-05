//
// Created by dekel on 09/11/2019.
//
#include "UnaryOperator.h"
using namespace std;
/*
Plus constructor.
*/
UPlus::UPlus(Expression *exp) {
    this->expression = exp;
}
/*
Calculates expression with plus.
*/
float UPlus::calculate() {
    return this->expression->calculate();
}
/*
Minus constructor.
*/
UMinus::UMinus(Expression *exp) {
    this->expression = exp;
}
/*
Calculates expression with minus.
*/
float UMinus::calculate() {
    return this->expression->calculate() * (-1);
}