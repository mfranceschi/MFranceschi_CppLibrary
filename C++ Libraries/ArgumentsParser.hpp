//
// Created by mfran on 25/12/2019.
//

#ifndef MYWORKS_TEST0_ARGUMENTSPARSER_HPP
#define MYWORKS_TEST0_ARGUMENTSPARSER_HPP

#include <exception>
#include <list>
#include <map>

class InvalidMultiItemsArgument : std::exception
{
public:
    inline explicit InvalidMultiItemsArgument(int index) :
            std::exception(),
            indexOfProblematicItem(index) {}
    const char* what() const noexcept override;

protected:
    int indexOfProblematicItem;
};

class UnfinishedMultiItemsArgument : InvalidMultiItemsArgument
{
public:
    inline explicit UnfinishedMultiItemsArgument(int index) :
        InvalidMultiItemsArgument(index) {}
    const char* what() const noexcept override;
};

enum class ValueType {
    BOOLEAN = 1,
    INTEGER = 2,
    LIST_STRING = 3
};

struct Argument {
    bool present;
    ValueType valueType;
    union {
        int integer_v; // For INTEGER
        bool boolean_v; // For BOOLEAN
        struct { // For LIST_STRING
            size_t length;
            const char** elements;
        };
    };
};

class ArgumentsParser {
public:
    // INNER TYPES
    using _Results = std::map<const char*, Argument>;

    // PUBLIC METHODS
    const ArgumentsParser& parse(int nargs, const char **the_args); // Parses the user input and returns current instance.
    inline const _Results& get() const; // Returns the results.
    void free(); // Call this to free the resources from the results.

    // OPERATORS OVERLOADING
    inline const ArgumentsParser& operator () (int nargs, const char** the_args); // Proxy for parse.

    // CONSTRUCTORS AND DESTRUCTOR
    ArgumentsParser() = default;
    ArgumentsParser(const ArgumentsParser&) = delete;
    ArgumentsParser(const ArgumentsParser&&) = delete;
    ArgumentsParser& operator= (const ArgumentsParser&) = delete;
    ArgumentsParser&& operator= (const ArgumentsParser&&) = delete;
    virtual ~ArgumentsParser();

private:
    // PROTECTED METHODS
    void parseInternal(int begin, int end, const char **the_args);
    void _getArgumentsAsIntegers(int begin, int end, const char **the_args, const std::list<std::pair<int, int>> &lomia);
    void computeMultiItemsArguments(int begin, int end, const char **the_args,
                                    std::list<std::pair<int, int>> &lomia);

    // PROTECTED ATTRIBUTES
    _Results results;
};

// OTHER DEFINITIONS
const ArgumentsParser::_Results &ArgumentsParser::get() const {
    return results;
}

const ArgumentsParser& ArgumentsParser::operator () (int nargs, const char** the_args) {
    return parse(nargs, the_args);
}

#endif //MYWORKS_TEST0_ARGUMENTSPARSER_HPP
