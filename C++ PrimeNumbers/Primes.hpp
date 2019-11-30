//
// Created by mfran on 16/11/2019.
//

#ifndef PRIMENUMBERSWINDOW_PRIMES_HPP
#define PRIMENUMBERSWINDOW_PRIMES_HPP

typedef unsigned long Number_t;

class Primes {
public:
    bool GetIsPrime();
    explicit Primes(Number_t);
private:
    Number_t number;
    bool internalGetIsPrime();
};


#endif //PRIMENUMBERSWINDOW_PRIMES_HPP
