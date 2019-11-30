//
// Created by mfran on 16/11/2019.
//

#include "Controller.hpp"
#include "QtDialogWindow.hpp"

using _Valid = NumberValidity;
using _WindowImplementation = QtDialogWindow;

Controller::Controller() :
    currentNumber(0),
    windowParams()
{
    windowParams.call_on_update = [&] () { Update();};
    window = new _WindowImplementation(windowParams);
    window->SetVisible(true);
    Update();
}

void Controller::NewNumber(unsigned long in) {
    currentNumber = Primes(in);
    window->SetAnswer(currentNumber.GetIsPrime());
}

Number_t Controller::StringToInt(const std::string& str) noexcept(false) {
    for (const auto& item : str) {
        if (!isdigit(item))
            throw _Valid(_Valid::INCORRECT_INPUT);
    }

    try {
        long result = std::stol(str);
        if (result >= 0) {
            return result;
        } else {
            throw _Valid(_Valid::INCORRECT_INPUT);
        }
    } catch (const std::invalid_argument&) {
        throw _Valid(_Valid::INCORRECT_INPUT);
    } catch (const std::out_of_range&) {
        throw _Valid(_Valid::ERROR_TOO_BIG);
    }
}

void Controller::Update() {
    std::string userInput = window->GetNumber();
    try {
        Number_t userNumber = StringToInt(userInput);
        window->SetNumberValid(_Valid::VALID);
        NewNumber(userNumber);
    } catch (const _Valid& err) {
        window->SetNumberValid(err);
    }
}
