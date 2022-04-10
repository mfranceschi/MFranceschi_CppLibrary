#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <system_error>

namespace MF
{
    namespace Windows
    {
        /**
         * Displays a message box with the given text, then terminates the program.
         */
        void ShowErrorMessage(const char *text);

        std::system_error GetCurrentSystemError();

        /**
         * Generates a newly-allocated (using the "new" operator) wchar_t array from the given
         * Multi-Bytes string.
         */
        std::wstring ConvertString(const char *utf8String);

        /// Sets or unsets the "inherit" flag to the given handle.
        void MakeHandleInheritable(void *handle, bool inherit = true);

        class FileAttributes {
           public:
            using Type = unsigned long;

            FileAttributes();
            FileAttributes(Type value);

            Type get() const;
            FileAttributes &set(Type value);

            bool isInvalid() const;
            bool isValid() const;

            bool isDirectory() const;
            bool isFile() const; /// defined as "not a directory"

           private:
            Type dWord;
        };

        static inline FileAttributes makeFileAttributes(FileAttributes::Type value) {
            FileAttributes instance(value);
            if (instance.isInvalid()) {
                throw std::invalid_argument("Provided file attributes are invalid");
            }
            return instance;
        }
    } // namespace Windows
} // namespace MF
