//
// Created by dekel on 09/11/2019.
//
#include "Value.h"

/*
Constructor of value with a given double.
*/
Value::Value(float val) {
    this->value = val;
}

/*
Calculates the value of the number
*/
float Value::calculate() {
    return this->value;
}