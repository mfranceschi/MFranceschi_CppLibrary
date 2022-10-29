//
// Created by Utilisateur on 28/10/2022.
//

#include "MF/SystemErrors.hpp"
#include "MF/Windows.hpp"

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
                const CharT** vectorToArrayOfArrays(
                    const std::vector<std::basic_string<CharT>>& strings) {
                    if (strings.empty()) {
                        return nullptr;
                    }

                    const CharT** arrayOfArrays = new const CharT*[strings.size()];
                    for (size_t i = 0; i < strings.size(); i++) {
                        CharT* currentArray = new CharT[strings[i].length() + 1];
                        copyString(strings[i], currentArray);
                        arrayOfArrays[i] = currentArray;
                    }
                    return arrayOfArrays;
                }
            } // namespace internals

#define DUMMY_GET(Thing_To_Get, Return_Type) \
    Return_Type EventRecord::get##Thing_To_Get() const { return getBufferAsRecord()->Thing_To_Get; }

            DUMMY_GET(Length, size_t)
            DUMMY_GET(RecordNumber, DWORD)
            DUMMY_GET(TimeGenerated, time_t)
            DUMMY_GET(TimeWritten, time_t)
            DUMMY_GET(EventID, DWORD)
            DUMMY_GET(EventCategory, WORD)

#undef DUMMY_GET

            EventType_e EventRecord::getEventType() const {
                return static_cast<EventType_e>(getBufferAsRecord()->EventType);
            }

            SID* EventRecord::getMaybeUserSid() const {
                auto record = getBufferAsRecord();
                return record->UserSidLength == 0 ? nullptr
                                                  : reinterpret_cast<SID*>(
                                                        static_cast<PBYTE>(buffer) +
                                                        std::ptrdiff_t(record->UserSidOffset));
            }

            RawData EventRecord::getData() const {
                return RawData{
                    getBufferAsRecord()->DataLength,
                    static_cast<PBYTE>(buffer) + std::ptrdiff_t(getBufferAsRecord()->DataOffset)};
            }

            wchar_t* EventRecord::getMaybeFirstString() const {
                return getNumberOfStrings() != 0
                           ? reinterpret_cast<wchar_t*>(
                                 static_cast<PBYTE>(buffer) +
                                 std::ptrdiff_t(getBufferAsRecord()->StringOffset))
                           : nullptr;
            }

            size_t EventRecord::getNumberOfStrings() const {
                return getBufferAsRecord()->NumStrings;
            }

            EVENTLOGRECORD* EventRecord::getBufferAsRecord() const {
                return static_cast<EVENTLOGRECORD*>(buffer);
            }

            EventRecord::EventRecord(LPVOID buffer) : buffer(buffer) {
            }

            EventRecord::~EventRecord() {
                free(buffer);
            }

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

            void EventLogWriter::reportEvent(const EventToWrite<char>& eventToWrite) {
                internals::ArrayOfArrays<const char> arrayOfArrays{
                    internals::vectorToArrayOfArrays(eventToWrite.strings),
                    eventToWrite.strings.size()};
                WORD eventType = eventTypeToWord(eventToWrite.eventType);

                BOOL success = ReportEventA(
                    hEventSource, eventType, eventToWrite.eventCategory, eventToWrite.eventId,
                    eventToWrite.userId, arrayOfArrays.nbItems, eventToWrite.rawData.size,
                    arrayOfArrays.pointer, eventToWrite.rawData.pointer);
                Win32::throwCurrentSystemErrorIf(success == FALSE);
            }

            void EventLogWriter::reportEvent(const EventToWrite<wchar_t>& eventToWrite) {
                internals::ArrayOfArrays<const wchar_t> arrayOfArrays{
                    internals::vectorToArrayOfArrays(eventToWrite.strings),
                    eventToWrite.strings.size()};
                WORD eventType = eventTypeToWord(eventToWrite.eventType);

                BOOL success = ReportEventW(
                    hEventSource, eventType, eventToWrite.eventCategory, eventToWrite.eventId,
                    eventToWrite.userId, arrayOfArrays.nbItems, eventToWrite.rawData.size,
                    arrayOfArrays.pointer, eventToWrite.rawData.pointer);
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

            EventLogReader::EventLogReader(
                const std::string& sourceName, const std::string& UNCServerName)
                : handle(OpenEventLogA(
                      UNCServerName.empty() ? nullptr : UNCServerName.c_str(),
                      sourceName.c_str())) {
                Win32::throwCurrentSystemErrorIf(handle == nullptr);
            }

            EventLogReader::EventLogReader(
                const std::wstring& sourceName, const std::wstring& UNCServerName)
                : handle(OpenEventLogW(
                      UNCServerName.empty() ? nullptr : UNCServerName.c_str(),
                      sourceName.c_str())) {
                Win32::throwCurrentSystemErrorIf(handle == nullptr);
            }

            std::unique_ptr<EventRecord> EventLogReader::readOneEventForward() {
                return readOneEvent(EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ);
            }

            std::unique_ptr<EventRecord> EventLogReader::readOneEventBackwards() {
                return readOneEvent(EVENTLOG_SEQUENTIAL_READ | EVENTLOG_BACKWARDS_READ);
            }

            std::unique_ptr<EventRecord> EventLogReader::readOneEventAtOffset(DWORD offset) {
                return readOneEvent(EVENTLOG_SEEK_READ, offset);
            }

            void EventLogReader::backup(const std::string& backupFileName) {
                BOOL success = BackupEventLogA(handle, backupFileName.c_str());
                Win32::throwCurrentSystemErrorIf(success == FALSE);
            }

            void EventLogReader::backup(const std::wstring& backupFileName) {
                BOOL success = BackupEventLogW(handle, backupFileName.c_str());
                Win32::throwCurrentSystemErrorIf(success == FALSE);
            }

            void EventLogReader::clear() {
                BOOL success = ClearEventLog(handle, nullptr);
                Win32::throwCurrentSystemErrorIf(success == FALSE);
            }

            void EventLogReader::notifyChangeEvent(HANDLE eventObject) {
                BOOL success = NotifyChangeEventLog(handle, eventObject);
                Win32::throwCurrentSystemErrorIf(success == FALSE);
            }

            DWORD EventLogReader::getOldestEventRecordNumber() {
                DWORD result = 0;
                BOOL success = GetOldestEventLogRecord(handle, &result);
                Win32::throwCurrentSystemErrorIf(success == FALSE);
                return result;
            }

            DWORD EventLogReader::getNumberOfRecords() {
                DWORD result = 0;
                BOOL success = GetNumberOfEventLogRecords(handle, &result);
                Win32::throwCurrentSystemErrorIf(success == FALSE);
                return result;
            }

            bool EventLogReader::isFull() {
                EVENTLOG_FULL_INFORMATION information;
                DWORD bytesNeeded = 0;

                BOOL success = GetEventLogInformation(
                    handle, EVENTLOG_FULL_INFO, &information, sizeof(information), &bytesNeeded);
                Win32::throwCurrentSystemErrorIf(success == FALSE);

                return information.dwFull == TRUE;
            }

            HANDLE EventLogReader::getHandle() const {
                return handle;
            }

            std::unique_ptr<EventRecord> EventLogReader::readOneEvent(DWORD flags, DWORD offset) {
                LPVOID buffer = this;
                DWORD bufferSize = 1;
                DWORD nbBytesRead = 0;
                DWORD minNumberOfBytesNeeded = 1;

                BOOL success = ReadEventLogA(
                    handle, flags, offset, buffer, bufferSize, &nbBytesRead,
                    &minNumberOfBytesNeeded);
                if (success != 0u) {
                    throw std::runtime_error(
                        "Call to 'ReadEventLogA' with 1 byte to read has succeeded!");
                }
                auto errorCode = Win32::getCurrentErrorCode();
                if (errorCode != ERROR_INSUFFICIENT_BUFFER) {
                    throw Win32::getSystemErrorForErrorCode(errorCode);
                }

                buffer = malloc(minNumberOfBytesNeeded);
                Errno::throwCurrentSystemErrorIf(buffer == nullptr);
                bufferSize = minNumberOfBytesNeeded;

                success = ReadEventLogA(
                    handle, flags, 0, buffer, bufferSize, &nbBytesRead, &minNumberOfBytesNeeded);
                Errno::throwCurrentSystemErrorIf(success == FALSE);

                return std::make_unique<EventRecord>(buffer);
            }

            EventLogReader::~EventLogReader() {
                CloseEventLog(handle);
            }
        } // namespace EventLog
    } // namespace Windows
} // namespace MF
