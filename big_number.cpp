#include "big_number.h"
#include <vector>

BigNumber::BigNumber()
{
    number_.push_back(0);
}

std::string BigNumber::AsString()
{
    std::string res;

    if (number_.size() < 10) {
        for(int i = 0; i < number_.size(); ++i) {
            res.push_back(number_[i] + '0');
        }
    }
    else {
        res.push_back(number_[0] + '0');
        res += "*10^" + std::to_string(number_.size() - 1);

    }

    return res;
}

BigNumber BigNumber::operator+(const BigNumber& second)
{
    BigNumber res;

    if (this->number_.size() >= second.number_.size()) {
        int64_t min = second.number_.size() - 1;
        int64_t max = this->number_.size() - 1;

        res.number_.resize(max + 2);

        for (; min >= 0; --min, --max) {
            char n = this->number_[max] + second.number_[min] + res.number_[max + 1];
            if (n < 10) {
                res.number_[max + 1] = n;
            }
            else {
                res.number_[max + 1] = n - 10;
                res.number_[max] = 1;
            }
        }

        for (; max >= 0; --max) {
            char n = this->number_[max] + res.number_[max + 1];
            if (n < 10) {
                res.number_[max + 1] = n;
            }
            else {
                res.number_[max + 1] = n - 10;
                res.number_[max] = 1;
            }
        }
        if (res.number_[0] == 0) {
            res.number_.pop_front();
        }
    }
    else {
        int64_t max = second.number_.size() - 1;
        int64_t min = this->number_.size() - 1;

        res.number_.resize(max + 2);

        for (; min >= 0; --min, --max) {
            char n = this->number_[min] + second.number_[max] + res.number_[max + 1];
            if (n < 10) {
                res.number_[max + 1] = n;
            }
            else {
                res.number_[max + 1] = n - 10;
                res.number_[max] = 1;
            }
        }

        for (; max >= 0; --max) {
            char n = second.number_[max] + res.number_[max + 1];
            if (n < 10) {
                res.number_[max + 1] = n;
            }
            else {
                res.number_[max + 1] = n - 10;
                res.number_[max] = 1;
            }
        }
        if (res.number_[0] == 0) {
            res.number_.pop_front();
        }
    }

    return res;
}

BigNumber BigNumber::operator+=(int x)
{
    BigNumber big_x;

    big_x.toBig(x);
    *this = *this + big_x;

    return *this;
}

BigNumber BigNumber::operator=(const BigNumber& second)
{
    number_.resize(second.number_.size());
    std::copy(second.number_.begin(), second.number_.end(), number_.begin());

    return *this;
}



BigNumber BigNumber::toBig(int x)
{
    BigNumber res;

    if(x == 0)
        return res;

    res.number_.pop_front();

    for (; x > 0; x = x / 10) {
        res.number_.push_front(static_cast<char>(x % 10));
    }
    std::swap(*this, res);
    return *this;
}

BigNumber BigNumber::operator*(int x)
{
    if (x == 1) {
        return *this;
    }

    BigNumber res;
    res = *this;

    for (; x > 1; --x) {
        res = *this + res;
    }

    return res;
}

BigNumber BigNumber::operator*=(int x)
{
    *this = *this * x;

    return *this;
}





















