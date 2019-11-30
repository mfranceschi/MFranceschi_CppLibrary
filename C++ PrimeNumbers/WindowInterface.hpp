//
// Created by mfran on 16/11/2019.
//

#ifndef PRIMENUMBERSWINDOW_WINDOWINTERFACE_H
#define PRIMENUMBERSWINDOW_WINDOWINTERFACE_H

#include <string>
#include <functional>

// Construction parameters for the created window.
struct ConstructionParams
{
    // Texts
    const char* label_window_title = "Martin cherche les premiers !";
    const char* output_if_prime = "OUI";
    const char* output_if_not_prime = "NON";
    const char* textfield_initial_value = "1";
    const char* output_error_incorrect_input = "Saisie incorrecte : un entier >=0 est attendu.";
    const char* output_error_too_big = "Désolé, ce nombre est trop grand!";
    const char* label_input_number = "Saisissez un nombre : ";
    const char* label_before_answer = "Ce nombre est-il premier ?";

    // Other parameters
    std::function<void()> call_on_update;
};

enum class NumberValidity {
    INCORRECT_INPUT,
    ERROR_TOO_BIG,
    VALID
};

class WindowInterface
{
public:
    // Gets the content of the text field as text.
    virtual std::string GetNumber() const = 0;

    // Sets whether the number is acceptable or not.
    // It impacts the display and the contents of the answer label.
    virtual void SetNumberValid(const NumberValidity&) = 0;

    // Sets the answer label's text.
    virtual void SetAnswer(bool) = 0;

    // Sets the window visible or not.
    virtual void SetVisible(bool) = 0;

protected:
    // Returns true if the current number has been set as invalid.
    virtual bool IsCurrentlyInvalid() const = 0;
};

#endif //PRIMENUMBERSWINDOW_WINDOWINTERFACE_H
