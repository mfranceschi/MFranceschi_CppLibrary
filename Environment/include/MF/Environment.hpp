//
// Created by MartinF on 25/06/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_ENVIRONMENT_HPP
#define MFRANCESCHI_CPPLIBRARIES_ENVIRONMENT_HPP

#include <string>

namespace MF
{
    namespace Environment
    {
        /**
         * Sets the environment variable "name" to the value "value".
         * Erases if already exists.
         * The behaviour is non-portable when value = "".
         */
        void setEnv(const std::string& name, const std::string& value);

        /**
         * Deletes the environment variable "name" for the current process.
         * No-op if doesn't exists.
         * Throws if "name" is empty.
         */
        void unsetEnv(const std::string& name);

        /**
         * Returns the value of the environment variable "name".
         * Throws if doesn't exist.
         * Throws if "name" is empty.
         */
        std::string getEnv(const std::string& name);

        /**
         * Returns the value of the environment variable "name".
         * Returns "defaultValue" if doesn't exist.
         * Throws if "name" is empty.
         */
        std::string getEnvOrDefault(const std::string& name, const std::string& defaultValue);
    } // namespace Environment
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_ENVIRONMENT_HPP
