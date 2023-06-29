#pragma once

#include <string>
#include <deque>

class BigNumber
{
public:
    BigNumber();

    std::string AsString();

    BigNumber operator+(const BigNumber &second);
    BigNumber operator+=(int x);
    BigNumber operator=(const BigNumber &second);
    BigNumber toBig(int x);
    BigNumber operator*(int x);
    BigNumber operator*=(int x);

private:
    std::deque<char> number_;
};

