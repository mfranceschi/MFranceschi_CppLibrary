//
// Created by Utilisateur on 29/10/2022.
//

#include <ctime>
#include <iostream>

#include "MF/Windows.hpp"

using namespace MF::Windows::EventLog;

static const std::wstring appNameForWriting = L"MF_Lib EventLog acceptance testing";

std::wstring getTimestampAsWstring() {
    return std::to_wstring(time(nullptr));
}

void logInfoEvent() {
    EventLogWriter eventLogWriter(appNameForWriting);
    std::vector<std::wstring> strings = {L"Hello, World! Timestamp is " + getTimestampAsWstring()};
    eventLogWriter.info(strings);
}

void readEvents(const std::wstring& appName) {
    EventLogReader eventLogReader(appName);

    std::cout << "There are " << eventLogReader.getNumberOfRecords() << " records here in total."
              << std::endl;
    std::cout << "The oldest event has the record number "
              << eventLogReader.getOldestEventRecordNumber() << std::endl;
    std::cout << "Is full? " << std::boolalpha << eventLogReader.isFull() << std::endl;

    auto event = eventLogReader.readOneEventForward();
    /*
    std::cout << "Reading event number " << event->getRecordNumber() << std::endl;
    event = eventLogReader.readOneEventBackwards();
    std::cout << "Reading event number " << event->getRecordNumber() << std::endl;
     */
}

int main() {
    try {
        // logInfoEvent();
        readEvents(appNameForWriting); // (L"Application");
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
    }
    return 0;
}
