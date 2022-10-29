//
// Created by Utilisateur on 29/10/2022.
//

#include <ctime>

#include "MF/Windows.hpp"

using namespace MF::Windows::EventLog;

static const std::string appName = "MF_Lib EventLog acceptance testing";

std::string getTimestampAsString() {
    return std::to_string(time(nullptr));
}

void logInfoEvent() {
    EventLogWriter eventLogWriter(appName);
    std::vector<std::string> strings = {"Hello, World! Timestamp is " + getTimestampAsString()};
    eventLogWriter.info(strings);
}

int main() {
    logInfoEvent();
    return 0;
}
