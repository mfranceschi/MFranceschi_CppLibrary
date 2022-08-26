//
// Created by MartinF on 07/08/2022.
//
#include <climits>
#include <clocale>
#include <cstring>
#include <iostream>

#include "MF/SystemErrors.hpp"

using namespace MF::SystemErrors;

int main() {
    Errno::setCurrentErrorCode(0);
    setlocale(LC_ALL, "fr-FR");
    setSystemErrorMessagesLocalized(true);

    const std::string str = "3336402735171707160320";

    const auto result = strtol(str.c_str(), nullptr, 10);
    const auto errorCode = Errno::getCurrentErrorCode();

    std::cout << "Result should be equal to " << LONG_MAX << ": " << result << std::endl;
    std::cout << "Errno should be EINVAL=" << EINVAL << ": " << errorCode << std::endl;

    const auto sysError = Errno::getSystemErrorForErrorCode(errorCode);
    std::cout << "Error message is:" << sysError.what() << std::endl;
    /*
    std::cout << "strerror_l message is:"
              << strerror_l(errorCode, newlocale(LC_MESSAGES_MASK, "en_US.UTF-8", (locale_t)0))
              << std::endl;
              */

    return 0;
}
