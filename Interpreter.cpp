//
// Created by dekel on 10/11/2019.
//
#include "Interpreter.h"
#include "Value.h"
#include <list>

using namespace std;

/*
Interpreter constructor
*/
Interpreter::Interpreter() {}

/*
Check if the input is illegal math expression
*/
void Interpreter::wrongInput(string str) {
    // Throw exception if an empty expression was given
    if (str.size() == 0) {
        throw "illegal math expression";
    }
    // Throw exception if the expression contains only one operator without numbers
    if (str.size() == 1) {
        auto it = Initializer::variableValue.find(str);
        if (isDigit(str[0]) || it != Initializer::variableValue.end()) {
            return;
        } else {
            throw "illegal math expression";
        }
    }
    // Throw exception if there are 2 operators one after the other
    string c1, c2;
    size_t i = 0;
    for (i = 0; i < str.size() - 1; i++) {
        c1 = str[i];
        c2 = str[i + 1];
        if ((isOperator(c1)) && (isOperator(c2))) {
            throw "illegal math expression";
        } else if ((str[i] == ')') && (str[i + 1] == '(')) {
            throw "illegal math expression";
        }
    }
    return;
}

/*
Check if there are illegal variables assignment
*/
void Interpreter::wrongVar(string args) {
    // Throw exception if an empty expression was given
    if (args.size() == 0) {
        throw "illegal variable assignment!";
    }
    if (isDigit(args[0])) {
        throw "illegal variable assignment!";
    }
    if ((args[(args.size() - 1)]) == '=') {
        throw "illegal variable assignment!";
    }
    size_t i = 0;
    for (i = 0; i < args.size() - 1; i++) {
        char c = args[i];
        if ((c == '=') && (!isDigit(args[i + 1]))) {
            throw "illegal variable assignment!";
        }
        if ((i > 1) && (isDigit(args[i - 1]))) {
            if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A')) {
                throw "illegal variable assignment!";
            }
        }
        if ((c == ';') && (isDigit(args[i + 1]))) {
            throw "illegal variable assignment!";
        }
        if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A') || (isDigit(c)) ||
            (c == '=') || (c == ';') || (c == '.') || (c == '_')) {
            continue;
        } else {
            throw "illegal variable assignment!";
        }
    }
    // Throw exception if there are 2 operators one after the other
    string c1, c2;
    size_t p = 0;
    for (p = 0; p < args.size(); p++) {
        c1 = args[p];
        c2 = args[p + 1];
        if ((isOperator(c1)) && (isOperator(c2))) {
            throw "illegal variable assignment!";
        } else if ((args[i] == ')') && (args[i + 1] == '(')) {
            throw "illegal variable assignment!";
        }
    }
}

Expression *Interpreter::interpret() {
    // Throw exception if wrong expression was given
    throw "illegal math expression";
}

/*
Evaluate the expression by split into tokens, conversion to Postfix format, and calculating the numerical
value of the expression.
*/
Expression *Interpreter::interpret(string str) {
    string result = "";
    this->infix = str;
    // Throw exception if wrong expression was given
    wrongInput(this->infix);
    // Replace variables names with their values
    evaluateVars();
    // If only numbers were given
    if (this->infix.size() == 1) {
        // Convert the char in the vars to double
        this->exp = new Value(stod(this->infix));
        // Split the expression if isn't a single number
    } else {
        for (size_t i = 0; i <= this->infix.size() - 1; i++) {
            char c = this->infix[i];
            if (isDigit(c) || c == '.') {
                if (i == (this->infix.size() - 1)) {
                    result += this->infix[i];
                    return new Value(stod(result));
                }
                result += this->infix[i];
                continue;
            }
            if ((c == '(') || (c == ')')) {
                if (i == (this->infix.size() - 1)) {
                    return new Value(stod(result));
                }
                continue;
            } else {
                break;
            }
        }
        divide();
        // Cheack if there are negative numbers
        negativeUnaryExp();
        // Convert to postfix
        string postfix = convertToPostfix();
        // Convert from postfix to numerical expression
        this->exp = fromPostfixToExpr(postfix);
    }
    //this->initializer->variableValue.clear();
    return this->exp;
}

/*
Replacing variables names with their values in case where given.
*/
void Interpreter::evaluateVars() {
    map<string, float>::iterator iter;

    if (Initializer::variableValue.empty()) {
        return;
    }
    // Go on all the variables
    for (iter = Initializer::variableValue.begin(); iter != Initializer::variableValue.end(); iter++) {
        string var = iter->first;
        string value = to_string(iter->second);
        // Replace all its appearances in the expression
        while (this->infix.find(var) != string::npos) {
            int pos = this->infix.find(var);
            this->infix.replace(pos, var.size(), value);
        }
    }
}

/*
Splitting words into map of variables.
*/
void Interpreter::setVariables() {
    throw "illegal variable assignment!";
}

/*
Splitting words into map of variables.
*/
void Interpreter::setVariables(string input) {
    size_t pos = 0;
    size_t posEnd = 0;
    string token;
    string var, valueStr;
    float value;
    wrongVar(input);
    while ((pos = input.find("=")) != string::npos) {
        var = input.substr(0, pos);
        if ((var.find(".")) != string::npos) {
            throw "illegal variable assignment!";
        }
        input.erase(0, pos + 1);
        if ((posEnd = input.find(";")) != string::npos) {
            valueStr = input.substr(0, posEnd);
            for (size_t q = 0; q < valueStr.size(); q++) {
                char c = valueStr[q];
                if (isDigit(c) || c == '.') {
                    continue;
                }
                throw "illegal variable assignment!";
            }
            value = stof(valueStr);
            input.erase(0, posEnd + 1);
        } else {
            value = stof(input);
        }
        if (Initializer::variableValue.empty()) {
            Initializer::updateValue(var, value);
        } else {
            auto k = Initializer::variableValue.find(var);
            if (k == Initializer::variableValue.end()) {
                Initializer::updateValue(var, value);
            } else {
                Initializer::updateValue(var, value);
            }
        }
        //if line.find() doesn't find delimiter, it returns npos
        if (pos == string::npos) {
            break;
        }
    }
}

/*
Separate the given expression with spaces between each char.
*/
void Interpreter::divide() {
    string result = "";
    // Go on all the expression
    for (size_t i = 0; i < this->infix.size(); i++) {
        // If is operator or a parenthesis put space in between
        if ((this->infix[i] == '+') || (this->infix[i] == '-') || (this->infix[i] == '*') || (this->infix[i] == '/') ||
            (this->infix[i] == '(') || (this->infix[i] == ')')) {
            result += this->infix[i];
            result += " ";
            // Ignore spaces
        } else if (this->infix[i] == ' ') {
            continue;
            // If is digit
        } else if (isDigit(this->infix[i])) {
            // If is the last char in the expression
            if (i + 1 >= this->infix.length()) {
                result += this->infix[i];
                // Else, put space in between or write '.' in case of float number
            } else {
                if ((isDigit(this->infix[i + 1])) || (infix[i + 1] == '.')) {
                    result += this->infix[i];
                } else {
                    result += this->infix[i];
                    result += " ";
                }
            }
        } else if (this->infix[i] == '.') {
            result += this->infix[i];
        }
    }
    // Remove unnecessary space from the end of the expression
    if (result[result.size() - 1] == ' ') {
        result.erase(result.begin() + (result.length() - 1));
    }
    // Set the divide infix to the result
    this->infix = result;
}

/*
Returns if the given char is a number.
*/
bool Interpreter::isDigit(char c) {
    return (c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') || (c == '5') || (c == '6') ||
           (c == '7') || (c == '8') || (c == '9');
}

/*
Distinguish the negative numbers and interpret the expression correctly by add zero, (, ).
*/
void Interpreter::negativeUnaryExp() {
    // Split the tokens to a vector of string
    vector<string> tokens = splitByDelimeter(this->infix, " ");
    // If there is '--' convert to +
    for (size_t p = 0; p < tokens.size() - 1; p++) {
        if ((tokens[p] == "-") && (!isNumber(tokens[p + 1])) && tokens[p + 2] == "-") {
            if (p != 0) {
                if (isNumber(tokens[p - 1])) {
                    continue;
                }
            }
            tokens.erase(tokens.begin() + p);
            tokens.erase(tokens.begin() + p + 1);
        }
    }
    // If the first token is "-"
    if (tokens[0] == "-") {
        if (tokens[1] == "(") {
            tokens.insert(tokens.begin(), "0");
            //if is -x then rewrite it as (0-x)
        } else if (isNumber(tokens[1])) {
            tokens.insert(tokens.begin() + 2, ")");
            tokens.insert(tokens.begin(), "0");
            tokens.insert(tokens.begin(), "(");
            // If there is '--' delete it from the string, convert it to '+'
        } else if (tokens[1] == "-") {
            tokens.erase(tokens.begin());
            tokens.erase(tokens.begin());
        }
    }
    // Cheack if there is "-" by over all the string
    size_t j = 0, t = 0;
    for (size_t i = 1; i < tokens.size() - 1; i++) {
        if ((tokens[i] == "-") && (!isNumber(tokens[i - 1]))) {
            if (tokens[i + 1] == "(") {
                tokens.insert(tokens.begin() + i, "(");
                tokens.insert(tokens.begin() + i + 1, "0");
                j = i + 2;
                while (tokens[j] != ")") {
                    j++;
                }
                tokens.insert(tokens.begin() + j + 1, ")");
                i += 2;
            } else if (tokens[i - 1] == "(") {
                tokens.insert(tokens.begin() + i, "(");
                tokens.insert(tokens.begin() + i + 1, "0");
                t = i + 2;
                while (tokens[t] != ")") {
                    t++;
                }
                tokens.insert(tokens.begin() + t + 1, ")");
                i += 2;
            } else if (isNumber(tokens[i + 1])) {
                tokens.insert(tokens.begin() + i - 1, "(");
                tokens.insert(tokens.begin() + i, "0");
                t = i + 2;
                while (tokens[t] != ")") {
                    t++;
                }
                tokens.insert(tokens.begin() + t + 1, ")");
                i += 2;
            }
        }
    }
    // Insert the result into a string
    string result;
    for (size_t i = 0; i < tokens.size(); i++) {
        result += tokens[i];
        result += " ";
    }
    // Remove unnecessary spaces
    result.erase(result.begin() + (result.length() - 1));
    this->infix = result;
}

/*
Splits the string by a given delimeter
*/
vector<string> Interpreter::splitByDelimeter(string str, string delimeter) {
    vector<string> tokens = {};
    size_t pos;
    while ((pos = str.find(delimeter))) {
        tokens.push_back(str.substr(0, pos));
        str.erase(0, pos + 1);
        // If the delimiter wasn't found in the string
        if (pos == string::npos) {
            break;
        }
    }
    return tokens;
}

/*
Returns if the given infix token is a number
*/
bool Interpreter::isNumber(string token) {
    // If the first char is minus
    if (token[0] == '-') {
        // If the token size > 1, it's a negative number
        return (token.size() >= 2);
    }
    // Else, its is a number only if the first character is a digit
    return isDigit(token[0]);
}

/*
Shunting-Yard algorithm to convert the expression from infix form to postfix.
*/
string Interpreter::convertToPostfix() {
    list<string> queue = {};
    list<string> stack = {};
    // Split the infix into vector of tokens
    vector<string> tokens = splitByDelimeter(this->infix, " ");
    // Go over all the vector
    for (auto &token : tokens) {
        // If is a number
        if (isNumber(token)) {
            queue.push_back(token);
            // If is an operator
        } else if (isOperator(token)) {
            // Pop onto the queue while there is an operator with greater precedence on the top of the stack
            while (!stack.empty()) {
                if (isOperator(stack.back())) {
                    if (operationSign(stack.back()) > operationSign(token)) {
                        queue.push_back(stack.back());
                        stack.pop_back();
                        continue;
                    }
                }
                break;
            }
            // Push to the stack
            stack.push_back(token);
            // Push onto the stack if is left parenthesis
        } else if (token == "(") {
            stack.push_back(token);
            // If is right parenthesis
        } else if (token == ")") {
            // While there is not "(" on the top of the stack, pop operator from the stack onto the queue
            while (!stack.empty()) {
                if (stack.back() != "(") {
                    queue.push_back(stack.back());
                    stack.pop_back();
                    continue;
                }
                break;
            }
            stack.pop_back();
        }
    }
    // While there is an operator at the top of the stack
    while (!stack.empty()) {
        if (isOperator(stack.back())) {
            queue.push_back(stack.back());
        }
        // Pop operator from stack onto the queue
        stack.pop_back();
    }
    // Insert the tokens into a string
    string result;
    for (auto &element : queue) {
        result += element;
        result += " ";
    }
    // Delete unnecessary spaces from the end
    result.erase(result.begin() + (result.size() - 1));
    return result;
}

/*
Returns if the given token is an operator
*/
bool Interpreter::isOperator(string token) {
    return ((token == "+") || (token == "-") || (token == "*") || (token == "/") || (token == "="));
}

/*
Constructing an expression tree from the postfix form.
*/
Expression *Interpreter::fromPostfixToExpr(string postfix) {
    vector<string> tokens;
    string str = postfix;
    // Split the postfix into tokens
    tokens = splitByDelimeter(str, " ");
    list<string> stack = {};
    double result;
    // From the left to the right if it is a number insert to the queue,
    // else, if it is an operator pop the last two numbers from the stack.
    // Then, run the operator on them and put the result in the stack.
    for (auto &token : tokens) {
        if (token == "+") {
            Expression *right = new Value(stod(stack.back()));
            stack.pop_back();
            Expression *left = new Value(stod(stack.back()));
            stack.pop_back();
            result = (new Plus(left, right))->calculate();
            delete left;
            delete right;
            stack.push_back(to_string(result));
        } else if (token == "-") {
            Expression *right = new Value(stod(stack.back()));
            stack.pop_back();
            Expression *left = new Value(stod(stack.back()));
            stack.pop_back();
            result = (new Minus(left, right))->calculate();
            stack.push_back(to_string(result));
            delete left;
            delete right;
        } else if (token == "*") {
            Expression *right = new Value(stod(stack.back()));
            stack.pop_back();
            Expression *left = new Value(stod(stack.back()));
            stack.pop_back();
            result = (new Mul(left, right))->calculate();
            stack.push_back(to_string(result));
            delete left;
            delete right;
        } else if (token == "/") {
            Expression *right = new Value(stod(stack.back()));
            stack.pop_back();
            Expression *left = new Value(stod(stack.back()));
            stack.pop_back();
            result = (new Div(left, right))->calculate();
            stack.push_back(to_string(result));
            delete left;
            delete right;
        } else {
            stack.push_back(token);
        }
    }
    // Return the result expression
    return new Value(result);
}

/*
Returns the precedence of the given operator.
*/
int Interpreter::operationSign(string operation) {
    if ((operation == "+") || (operation == "-")) {
        return 1;
    } else {
        return 2;
    }
}