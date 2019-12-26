//
// Created by mfran on 25/12/2019.
//

#ifndef MYWORKS_TEST0_TESTSFORARGUMENTS_H
#define MYWORKS_TEST0_TESTSFORARGUMENTS_H

#include <map>

// Actual argument to create in the user response.
// User usage: just get this instance's "Value()" method
template <typename _ArgType>
class Argument
{
public:
    Argument(const _ArgType& new_value, bool present);
    inline bool IsPresent() const noexcept ; // Returns true if it is already present.
    inline const _ArgType& Value() const noexcept ; // Returns the current value if there is one.
protected:
    _ArgType value;
    bool present;
};

template<typename _ArgType>
const _ArgType &Argument<_ArgType>::Value() const noexcept {
    return value;
}

template<typename _ArgType>
bool Argument<_ArgType>::IsPresent() const noexcept {
    return present;
}

template <typename _ArgType>
class ArgumentsSettings
{
protected:
    const char* name;
    unsigned short minOccurs = 1; // Minimum required number of occurrences of the argument.
    unsigned short maxOccurs = 1; // Maximum required number of occurrences of the argument.
    unsigned short minFollowingParameters = 1;
    unsigned short maxFollowingParameters = 1;
};

class ArgumentParser
{
public:
    typedef void* Argument; // A returned argument.
    typedef void* ArgumentsSettings; // A setting for the future parsing.
    using _Results = std::map<const char*, Argument*>;

    const _Results& parse(int nargs, char** the_args);
    const _Results& getResults() const;
    void free(); // Call this to free the resources from the results.
protected:
    const _Results& results;


};

#endif //MYWORKS_TEST0_TESTSFORARGUMENTS_H
