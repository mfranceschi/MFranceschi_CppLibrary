//
// Created by mfran on 16/11/2019.
//

#ifndef PRIMENUMBERSWINDOW_CONTROLLER_H
#define PRIMENUMBERSWINDOW_CONTROLLER_H


#include "Primes.hpp"
#include "WindowInterface.hpp"

class Controller {
public:
    explicit Controller();

    // Using a valid > 0 number, computes and updates GUI.
    void NewNumber(unsigned long);

    // Gets the user input and calls NewNumber if valid.
    void Update();

private:
    Primes currentNumber;
    WindowInterface* window;
    ConstructionParams windowParams;

    // Converts str to number, throws on failure.
    static unsigned long StringToInt(const std::string&) noexcept(false);
};

#endif //PRIMENUMBERSWINDOW_CONTROLLER_H
