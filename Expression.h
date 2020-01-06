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
    //calculte method
    virtual float calculate() = 0;

    //destructor
    virtual ~Expression() = default;
};

#endif //EX3_EXPRESSION_H