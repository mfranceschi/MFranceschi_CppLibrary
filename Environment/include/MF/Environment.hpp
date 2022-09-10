//
// Created by MartinF on 25/06/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_ENVIRONMENT_HPP
#define MFRANCESCHI_CPPLIBRARIES_ENVIRONMENT_HPP

#include <string>
#include <vector>

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
        std::string getEnvOrDefault(const std::string& name, const std::string& defaultValue = "");

        /**
         * Returns true if the environment variable "name" exists, false otherwise.
         * Throws if "name" is empty.
         */
        bool hasEnv(const std::string& name);

        /**
         * Returns an array that is a copy of the names of all the environment variables for the current process.
         */
        std::vector<std::string> listNames();

        /**
         * Returns an array of <name, value> of all the environment variables for the current process.
         */
        std::vector<std::pair<std::string, std::string>> listAll();
    } // namespace Environment
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_ENVIRONMENT_HPP
