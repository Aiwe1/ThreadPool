#pragma once

#include "big_number.h"
#include <string>
#include <iostream>

namespace {

std::string Factorial(int x)
{
    if (x < 0) {
        return std::string("error");
    }

    BigNumber res;
    res += 1;

    for (; x > 0; --x) {
        res *= x;
    }

    return res.AsString();
}

std::string dblFactorial(int x)
{
    if (x < 0) {
        return std::string("error");
    }

    BigNumber res;
    res += 1;

    for (; x > 0; x -= 2) {
        res *= x;
    }

    return res.AsString();

}

std::string Fibonachi(int x)
{
    if (x <= 0) {
        return std::string("error");
    }
    if (x == 1) {
        return std::string("0");
    }
    if (x == 2) {
        return std::string("1");
    }
    if (x == 3) {
        return std::string("1");
    }

    BigNumber old;
    BigNumber value;
    value += 1;
    BigNumber hold;

    for (int n = 1; n < x; n++) {
        hold = value;
        value = value + old;
        old = hold;
    }

    return value.AsString();
}
}
