/* Martin Franceschi - MyWorks package - Toolbox module.
 *
 *
 */

//---------- Interface of module <Toolbox> (file Toolbox.h)
#if !defined(TOOLBOX_H)
#    define TOOLBOX_H

//--------------------------------------------------------------- Includes
#    include <algorithm>
#    include <functional>
#    include <sstream>
#    include <vector>

#    ifdef max
#        undef max
#    endif // max
#    ifdef min
#        undef min
#    endif // min

namespace Toolbox
{
    //------------------------------------------------------------------ Types

    // Simple "istringstream" extension for reading C-string
    // without copy and without end character.
    class InCharArrayStream : public std::istringstream {
       public:
        // The full string and the size.
        // Reading content[size-1] is ok, reading content[size] is undefined behavior.
        InCharArrayStream(const char* content, size_t size);

       protected:
        struct InCharArrayBuffer : public std::stringbuf {
            InCharArrayBuffer(const char* content, size_t size);
        };
        InCharArrayBuffer icab;
    };

    //-------------------------------------------------------------- Constants

    constexpr int INFERIOR = -1, EQUAL = 0, SUPERIOR = +1; // For comparisons.

    //////////////////////////////////////////////////////////////////  PUBLIC
    //------------------------------------------------------- Public functions

    // Returns the same C-string but converted in wchar_t*.
    // It is created using new[] --> please use delete[] after use!
    // Returns nullptr in case of failure.
    wchar_t* ToWchar_t(const char* source);

    // Returns the execution time for the given function.
    // It runs "iter" times and returns (total_time/iter).
    double TimeThis(size_t iter, const std::function<void(void)>&);

    // Returns the sign of the given number: -1, 0 or +1, as an int.
    template <typename type = long long>
    constexpr int Sign(type nbr);

    // Returns the value if it is valid or throws given object.
    template <typename value_type, typename exception_class>
    constexpr value_type Validate(
        value_type new_value,
        const exception_class& value_to_throw,
        std::function<bool(value_type)> validator);

    // Overloading of "Validate" for checking "new_value" is in the right range [min,max].
    template <typename value_type, typename exception_class>
    constexpr value_type Validate(
        value_type new_value,
        const exception_class& value_to_throw,
        value_type min,
        value_type max);

    template <typename value_type, typename exception_class>
    constexpr value_type Validate(
        value_type new_value, const exception_class& value_to_throw, bool result);

    template <typename content_type>
    inline bool SwapAndRemove(std::vector<content_type>&, size_t index);

    // Uses the system tool to display "Press any key to continue..." on the console and wait for
    // any user input. This is obviously a blocking call.
    inline void PressAnyKeyToContinue();

    // Like normal <cassert> assertion, but displays a more pretty (and optional) message on
    // failure. It may always evaluates the condition. No abort nor output if "NDEBUG" macro is
    // defined. Unfortunately, it does not print the __FILE__ and __LINE__ information.
    void PrettyAssertion(bool condition, const char* message = nullptr);

    // Runs the given code "iterations" times and dumps the memory leaks if there are some in the
    // debug output. If not MSVC, it does nothing nor runs the function.
    void Win_CheckForMemoryLeaks(
        const std::function<void()>& =
            []() {
            },
        size_t iterations = 1);
} // namespace Toolbox

//------------------------------------------------------ Other definitions
template <typename type>
constexpr int Toolbox::Sign(type nbr) {
    return !nbr ? 0 : nbr < static_cast<type>(0) ? -1 : +1;
}

template <typename value_type, typename exception_class>
constexpr value_type Toolbox::Validate(
    value_type new_value,
    const exception_class& value_to_throw,
    const std::function<bool(value_type)> validator) {
    if (validator(new_value))
        return new_value;
    else
        throw value_to_throw;
}

template <typename value_type, typename exception_class>
constexpr value_type Toolbox::Validate(
    value_type new_value, const exception_class& value_to_throw, value_type min, value_type max) {
    std::function<bool(value_type)> func = [&min, &max](value_type newvalue) -> bool {
        return !(newvalue < min || newvalue > max);
    };

    return Toolbox::Validate(new_value, value_to_throw, func);
}

template <typename value_type, typename exception_class>
constexpr value_type Toolbox::Validate(
    value_type new_value, const exception_class& value_to_throw, bool result) {
    if (result)
        return new_value;
    else
        throw value_to_throw;
}

template <typename content_type>
bool Toolbox::SwapAndRemove(std::vector<content_type>& vec, size_t index) {
    if (vec.empty() || index >= vec.size()) return false;
    if (index - 1 == vec.size()) {
        vec.pop_back();
        return true;
    }

    auto item = vec.begin() + index;
    auto end = vec.end();
    std::iter_swap(item, end);
    vec.pop_back();
    return true;
}

inline void Toolbox::PressAnyKeyToContinue() {
#    ifdef _WIN32
    system("pause");
#    else
    system("read");
#    endif
}

#endif // TOOLBOX_H
