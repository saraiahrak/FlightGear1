//
// Created by dekel on 09/11/2019.
//
#include "Variable.h"
using namespace std;
/*
Variable constructor.
*/
Variable::Variable(string var, float val) {
    this->variable = var;
    this->value = val;
}
// Calculates expressions
Variable &Variable::operator++() {
    this->value++;
    return *(this);
}
Variable &Variable::operator++(int) {
    this->value++;
    return *(this);
}
Variable &Variable::operator--() {
    this->value--;
    return *(this);
}
Variable &Variable::operator--(int) {
    this->value--;
    return *(this);
}
Variable &Variable::operator+=(float num) {
    this->value += num;
    return *(this);
}
Variable &Variable::operator-=(float num) {
    this->value -= num;
    return *(this);
}
float Variable::calculate() {
    return this->value;
}
// Destructor
Variable::~Variable() {
    delete this;
}