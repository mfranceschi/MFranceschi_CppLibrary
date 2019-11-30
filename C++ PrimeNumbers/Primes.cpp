//
// Created by mfran on 16/11/2019.
//

#include "Primes.hpp"

bool Primes::GetIsPrime() {
    return internalGetIsPrime();
}

Primes::Primes(Number_t in) :
    number(in)
{}

bool Primes::internalGetIsPrime() {
    // Code from https://en.wikipedia.org/wiki/Primality_test
    if (number <= 3)
    {
        return number > 1;
    }
    else if ((number % 2 == 0) || (number % 3 == 0))
    {
        return false;
    }

    Number_t i = 5l;
    while (i*i <= number)
    {
        if ((number % i == 0) || (number % (i+2) == 0))
        {
            return false;
        }
        i += 6;
    }

    return true;
}

