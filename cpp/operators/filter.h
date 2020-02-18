//
// Created by madhav on 12/26/19.
//

#ifndef TESTING_FILTER_H
#define TESTING_FILTER_H

#include <string>

using namespace std;

struct Literal {
};

struct EqExp {
};

struct Expression {
    union {
        EqExp eq;
        Literal lit;
    };
};

struct FilterDef {
    Expression exp;
};

class Filter {

};


#endif //TESTING_FILTER_H
