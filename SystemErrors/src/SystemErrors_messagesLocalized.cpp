//
// Created by MartinF on 03/07/2022.
//

#include <atomic>

#include "MF/SystemErrors.hpp"

namespace MF
{
    namespace SystemErrors
    {
        static std::atomic_bool& getTheValue() {
            static std::atomic_bool systemErrorMessagesLocalized(true);
            return systemErrorMessagesLocalized;
        }

        void setSystemErrorMessagesLocalized(bool newValue) {
            getTheValue() = newValue;
        }

        bool getSystemErrorMessagesLocalized() {
            return getTheValue().load();
        }
    } // namespace SystemErrors
} // namespace MF
