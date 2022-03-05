#include "string"

namespace MF {
    namespace Windows {
        /**
         * Displays a message box with the given text, then terminates the program.
         */
        void ShowErrorMessage(const char *text);

        /**
         * Generates a newly-allocated (using the "new" operator) wchar_t array from the given Multi-Bytes string.
         */
        std::wstring ConvertString(const char *utf8String);

        /// Sets or unsets the "inherit" flag to the given handle.
        void MakeHandleInheritable(void *handle, bool inherit = true);
    }
}