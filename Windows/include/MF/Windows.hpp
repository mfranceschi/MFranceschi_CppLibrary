#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include "MF/LightWindows.hpp"

namespace MF
{
    namespace Windows
    {
        /**
         * Displays a message box with the given text, then terminates the program.
         */
        void ShowErrorMessage(const char* text);

        /**
         * Generates a newly-allocated (using the "new" operator) wchar_t array from the given
         * Multi-Bytes string.
         */
        std::wstring ConvertString(const char* utf8String);

        class FileAttributes {
           public:
            FileAttributes() = delete;
            FileAttributes(DWORD value);

            DWORD get() const;
            FileAttributes& set(DWORD value);

            bool isInvalid() const;
            bool isValid() const;

            bool isDirectory() const;
            bool isFile() const; /// defined as "not a directory"

           private:
            DWORD dWord;
        };

        static inline FileAttributes makeFileAttributes(DWORD value) {
            FileAttributes instance(value);
            if (instance.isInvalid()) {
                throw std::invalid_argument("Provided file attributes are invalid");
            }
            return instance;
        }

        namespace EventLog
        {
            enum class EventType_e {
                eSUCCESS = EVENTLOG_SUCCESS,
                eINFORMATION = EVENTLOG_INFORMATION_TYPE,
                eWARNING = EVENTLOG_WARNING_TYPE,
                eERROR = EVENTLOG_ERROR_TYPE,
                eAUDIT_SUCCESS = EVENTLOG_AUDIT_SUCCESS,
                eAUDIT_FAILURE = EVENTLOG_AUDIT_FAILURE
            };

            inline WORD eventTypeToWord(EventType_e e) {
                return static_cast<WORD>(e);
            }

            struct RawData {
                std::size_t size = 0;
                void* pointer = nullptr;
            };

            template <typename CharT>
            struct EventToWrite {
                std::vector<std::basic_string<CharT>> strings;
                DWORD eventId = 0;
                WORD eventCategory = 0;
                EventType_e eventType = EventType_e::eINFORMATION;
                PSID userId = nullptr;
                RawData rawData;
            };

            struct EventRecord {
                size_t getLength() const;
                DWORD getRecordNumber() const;
                time_t getTimeGenerated() const;
                time_t getTimeWritten() const;
                DWORD getEventID() const;
                WORD getEventCategory() const;
                EventType_e getEventType() const;
                SID* getMaybeUserSid() const;
                RawData getData() const;
                wchar_t* getMaybeFirstString() const;
                size_t getNumberOfStrings() const;

                EVENTLOGRECORD* getBufferAsRecord() const;

                EventRecord(LPVOID buffer);
                ~EventRecord(); // Calls 'free'.

                const LPVOID buffer;
            };

            class EventLogWriter {
               public:
                explicit EventLogWriter(
                    const std::string& sourceName, const std::string& UNCServerName = "");

                explicit EventLogWriter(
                    const std::wstring& sourceName, const std::wstring& UNCServerName = L"");

                void info(
                    const std::vector<std::string>& strings,
                    DWORD eventId = 0,
                    WORD eventCategory = 0);

                void reportEvent(const EventToWrite<char>& eventToWrite);
                void reportEvent(const EventToWrite<wchar_t>& eventToWrite);

                bool isFull();
                HANDLE getHandle() const;

                ~EventLogWriter();

               private:
                const HANDLE hEventSource;
            };

            class EventLogReader {
               public:
                explicit EventLogReader(
                    const std::string& sourceName, const std::string& UNCServerName = "");

                explicit EventLogReader(
                    const std::wstring& sourceName, const std::wstring& UNCServerName = L"");

                std::unique_ptr<EventRecord> readOneEventForward();
                std::unique_ptr<EventRecord> readOneEventBackwards();
                std::unique_ptr<EventRecord> readOneEventAtOffset(DWORD offset);

                void backup(const std::string& backupFileName);
                void backup(const std::wstring& backupFileName);
                void clear();

                void notifyChangeEvent(HANDLE eventObject);
                DWORD getOldestEventRecordNumber();
                DWORD getNumberOfRecords();

                bool isFull();
                HANDLE getHandle() const;

                ~EventLogReader();

               private:
                std::unique_ptr<EventRecord> readOneEvent(DWORD flags, DWORD offset = 0);
                const HANDLE handle;
            };
        } // namespace EventLog

        namespace internals
        {
            template <typename ResourceType, typename Closer>
            class ResourceCloser {
               public:
                ResourceCloser(ResourceType resource, Closer closer)
                    : resource(resource), closer(closer) {
                }

                const ResourceType& get() const {
                    return resource;
                }

                ResourceType release() {
                    ResourceType theResource = resource;
                    resource = ResourceType();
                    closer = Closer();
                    return theResource;
                }

                virtual ~ResourceCloser() {
                    closer(resource);
                }

               private:
                Closer closer;
                ResourceType resource;
            };
        } // namespace internals

        class HandleCloser : public internals::ResourceCloser<HANDLE, decltype(&CloseHandle)> {
           public:
            HandleCloser(HANDLE handle)
                : ResourceCloser<HANDLE, decltype(&CloseHandle)>(handle, CloseHandle) {
            }

            bool isInvalid() const {
                return get() == INVALID_HANDLE_VALUE;
            }
        };

        class FileHandleCloser : public internals::ResourceCloser<HANDLE, decltype(&FindClose)> {
           public:
            FileHandleCloser(HANDLE handle)
                : ResourceCloser<HANDLE, decltype(&FindClose)>(handle, FindClose) {
            }

            bool isInvalid() const {
                return get() == INVALID_HANDLE_VALUE;
            }
        };
    } // namespace Windows
} // namespace MF
