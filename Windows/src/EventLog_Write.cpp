//
// Created by Utilisateur on 30/10/2022.
//
//
// Created by Utilisateur on 28/10/2022.
//

#include "MF/SystemErrors.hpp"
#include "MF/Windows.hpp"

#pragma comment(lib, "Advapi32.lib")

using namespace MF::SystemErrors;

namespace MF
{
    namespace Windows
    {
        namespace EventLog
        {
            namespace internals
            {
                void copyString(const std::string& source, char* destination) {
                    auto errnoReturned = strcpy_s(
                        destination, sizeof(destination) * (source.length() + 1), source.c_str());
                    if (errnoReturned != 0) {
                        throw std::runtime_error("Internal buffering error.");
                    }
                }

                void copyString(const std::wstring& source, wchar_t* destination) {
                    auto errnoReturned = wcscpy_s(
                        destination, sizeof(destination) * (source.length() + 1), source.c_str());
                    if (errnoReturned != 0) {
                        throw std::runtime_error("Internal buffering error.");
                    }
                }

                template <typename ItemType>
                struct ArrayOfArrays {
                    ItemType** pointer;
                    size_t nbItems;

                    ~ArrayOfArrays() {
                        for (size_t i = 0; i < nbItems; i++) {
                            delete[] pointer[i];
                        }
                        delete[] pointer;
                    }
                };

                template <typename CharT>
                CharT** vectorToArrayOfArrays(
                    const std::vector<std::basic_string<CharT>>& strings) {
                    if (strings.empty()) {
                        return nullptr;
                    }

                    CharT** arrayOfArrays = new CharT*[strings.size()];
                    for (size_t i = 0; i < strings.size(); i++) {
                        CharT* currentArray = new CharT[strings[i].length() + 1];
                        copyString(strings[i], currentArray);
                        arrayOfArrays[i] = currentArray;
                    }
                    return arrayOfArrays;
                }
            } // namespace internals

            EventLogWriter::EventLogWriter(
                const std::string& sourceName, const std::string& UNCServerName)
                : hEventSource(RegisterEventSourceA(
                      UNCServerName.empty() ? nullptr : UNCServerName.c_str(),
                      sourceName.c_str())) {
                Win32::throwCurrentSystemErrorIf(hEventSource == nullptr);
            }

            EventLogWriter::EventLogWriter(
                const std::wstring& sourceName, const std::wstring& UNCServerName)
                : hEventSource(RegisterEventSourceW(
                      UNCServerName.empty() ? nullptr : UNCServerName.c_str(),
                      sourceName.c_str())) {
                Win32::throwCurrentSystemErrorIf(hEventSource == nullptr);
            }

            void EventLogWriter::info(
                const std::vector<std::string>& strings, DWORD eventId, WORD eventCategory) {
                reportEvent(
                    EventToWrite<char>{strings, eventId, eventCategory, EventType_e::eINFORMATION});
            }

            void EventLogWriter::info(
                const std::vector<std::wstring>& strings, DWORD eventId, WORD eventCategory) {
                reportEvent(EventToWrite<wchar_t>{
                    strings, eventId, eventCategory, EventType_e::eINFORMATION});
            }

            void EventLogWriter::reportEvent(const EventToWrite<char>& eventToWrite) {
                internals::ArrayOfArrays<char> arrayOfArrays{
                    internals::vectorToArrayOfArrays(eventToWrite.strings),
                    eventToWrite.strings.size()};
                WORD eventType = eventTypeToWord(eventToWrite.eventType);
                auto stringsPointer = const_cast<const char**>(arrayOfArrays.pointer);

                BOOL success = ReportEventA(
                    hEventSource, eventType, eventToWrite.eventCategory, eventToWrite.eventId,
                    eventToWrite.userId, arrayOfArrays.nbItems, eventToWrite.rawData.size,
                    stringsPointer, eventToWrite.rawData.pointer);
                Win32::throwCurrentSystemErrorIf(success == FALSE);
            }

            void EventLogWriter::reportEvent(const EventToWrite<wchar_t>& eventToWrite) {
                internals::ArrayOfArrays<wchar_t> arrayOfArrays{
                    internals::vectorToArrayOfArrays(eventToWrite.strings),
                    eventToWrite.strings.size()};
                WORD eventType = eventTypeToWord(eventToWrite.eventType);
                auto stringsPointer = const_cast<const wchar_t**>(arrayOfArrays.pointer);

                BOOL success = ReportEventW(
                    hEventSource, eventType, eventToWrite.eventCategory, eventToWrite.eventId,
                    eventToWrite.userId, arrayOfArrays.nbItems, eventToWrite.rawData.size,
                    stringsPointer, eventToWrite.rawData.pointer);
                Win32::throwCurrentSystemErrorIf(success == FALSE);
            }

            bool EventLogWriter::isFull() {
                EVENTLOG_FULL_INFORMATION information;
                DWORD bytesNeeded = 0;

                BOOL success = GetEventLogInformation(
                    hEventSource, EVENTLOG_FULL_INFO, &information, sizeof(information),
                    &bytesNeeded);
                Win32::throwCurrentSystemErrorIf(success == FALSE);

                return information.dwFull == TRUE;
            }

            HANDLE EventLogWriter::getHandle() const {
                return hEventSource;
            }

            EventLogWriter::~EventLogWriter() {
                DeregisterEventSource(hEventSource);
            }
        } // namespace EventLog
    } // namespace Windows
} // namespace MF
