//
// Created by dekel on 09/11/2019.
//

#ifndef EX3_EXPRESSION_H
#define EX3_EXPRESSION_H


using namespace std;

/**
 * Expression Interface
 */
class Expression {

public:
    virtual float calculate() = 0;

    virtual ~Expression() {}
};

#endif //EX3_EXPRESSION_H