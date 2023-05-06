//
// Created by MartinF on 10/04/2023.
//

#if MF_WINDOWS
// #    include <tchar.h>

#    include "Command_Windows_commons.hpp"
#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Command
    {
        const StreamItem INVALID_STREAM_ITEM = INVALID_HANDLE_VALUE;

        /// Use this handle as a sample for inheritable handles.
        static SECURITY_ATTRIBUTES securityAttributesForInheritableHandles{
            sizeof(SECURITY_ATTRIBUTES), nullptr, true};

        SECURITY_ATTRIBUTES &getInheritableSecAttr() {
            return securityAttributesForInheritableHandles;
        }

        std::vector<char> makeCommandLine(
            const Filename_t *executable, const std::vector<std::string> *arguments) {
            std::basic_ostringstream<char> osStream;

            // TODO escape whitespaces to `\ `
            osStream << executable->c_str();
            if (!arguments->empty()) {
                osStream << " ";
                for (const auto &arg : *arguments) {
                    osStream << arg << " ";
                }
            }
            auto s = osStream.str();
            std::vector<char> result;
            result.reserve(s.size() + 1);

            errno_t errno1 = strncpy_s(result.data(), result.capacity(), s.c_str(), s.length());
            if (errno1) {
                throw MF::SystemErrors::Errno::getSystemErrorForErrorCode(errno1);
            }
            return result;
        }

        std::vector<wchar_t> makeCommandLine(
            const WideFilename_t *executable, const std::vector<std::wstring> *arguments) {
            std::basic_ostringstream<wchar_t> osStream;

            // TODO escape whitespaces to `\ `
            osStream << executable->c_str();
            if (!arguments->empty()) {
                osStream << " ";
                for (const auto &arg : *arguments) {
                    osStream << arg << " ";
                }
            }
            auto s = osStream.str();
            std::vector<wchar_t> result;
            result.reserve(s.size() + 1);

            errno_t errno1 = wcsncpy_s(result.data(), result.capacity(), s.c_str(), s.length());
            if (errno1) {
                throw MF::SystemErrors::Errno::getSystemErrorForErrorCode(errno1);
            }
            return result;
        }

        void populateStartupInfo(const CommandCall &commandCall, STARTUPINFOA &startupinfo) {
            ZeroMemory(&startupinfo, sizeof(startupinfo));
            startupinfo.cb = sizeof(startupinfo);
            startupinfo.dwFlags |= STARTF_USESTDHANDLES;
            startupinfo.hStdOutput =
                dynamic_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdOutChoice))
                    .getStreamItem(OutputStream_e::StdOut);
            startupinfo.hStdError =
                dynamic_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdErrChoice))
                    .getStreamItem(OutputStream_e::StdErr);
            startupinfo.hStdInput =
                dynamic_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice))
                    .getStreamItem();
        }

        void populateStartupInfo(const WideCommandCall &commandCall, STARTUPINFOW &startupinfo) {
            ZeroMemory(&startupinfo, sizeof(startupinfo));
            startupinfo.cb = sizeof(startupinfo);
            startupinfo.dwFlags |= STARTF_USESTDHANDLES;
            startupinfo.hStdOutput =
                dynamic_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdOutChoice))
                    .getStreamItem(OutputStream_e::StdOut);
            startupinfo.hStdError =
                dynamic_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdErrChoice))
                    .getStreamItem(OutputStream_e::StdErr);
            startupinfo.hStdInput =
                dynamic_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice))
                    .getStreamItem();
        }

        int getExitCode(ProcessItem processItem) {
            DWORD exitCode;
            BOOL success = GetExitCodeProcess(processItem, &exitCode);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(success == FALSE);
            return static_cast<int>(exitCode);
        }

        void makeHandleInheritable(HANDLE handle, bool yesOrNo) {
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
                SetHandleInformation(
                    handle, HANDLE_FLAG_INHERIT, yesOrNo ? HANDLE_FLAG_INHERIT : 0) == FALSE);
        }

        void closeH(HANDLE &handle) {
            if (handle != INVALID_HANDLE_VALUE) {
                CloseHandle(handle);
                handle = INVALID_HANDLE_VALUE;
            }
        }

        PipeStreams makePipeThatChildWillRead() {
            PipeStreams pipeStreams{0};
            BOOL result = CreatePipe(
                &pipeStreams.readFromPipe, &pipeStreams.writeToPipe, &getInheritableSecAttr(), 0);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!result);
            makeHandleInheritable(pipeStreams.readFromPipe, true);
            makeHandleInheritable(pipeStreams.writeToPipe, false);
            return pipeStreams;
        }

        PipeStreams makePipeThatChildWillWriteOn() {
            PipeStreams pipeStreams{0};
            BOOL result = CreatePipe(
                &pipeStreams.readFromPipe, &pipeStreams.writeToPipe, &getInheritableSecAttr(), 0);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!result);
            makeHandleInheritable(pipeStreams.readFromPipe, false);
            makeHandleInheritable(pipeStreams.writeToPipe, true);
            return pipeStreams;
        }

        StreamItem openFileToRead(const Filename_t &filename) {
            HANDLE handle = CreateFileA(
                filename.c_str(), GENERIC_READ, FILE_SHARE_READ, &getInheritableSecAttr(),
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(handle == INVALID_HANDLE_VALUE);
            return handle;
        }

        StreamItem openFileToRead(const WideFilename_t &filename) {
            HANDLE handle = CreateFileW(
                filename.c_str(), GENERIC_READ, FILE_SHARE_READ, &getInheritableSecAttr(),
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(handle == INVALID_HANDLE_VALUE);
            return handle;
        }

        StreamItem openFileToWrite(const Filename_t &filename) {
            HANDLE handle = CreateFileA(
                filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &getInheritableSecAttr(),
                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(handle == INVALID_HANDLE_VALUE);
            return handle;
        }

        StreamItem openFileToWrite(const WideFilename_t &filename) {
            HANDLE handle = CreateFileW(
                filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &getInheritableSecAttr(),
                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(handle == INVALID_HANDLE_VALUE);
            return handle;
        }

        StreamItem openNullFileToWrite() {
            HANDLE handle = CreateFile(
                TEXT("NUL"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                &getInheritableSecAttr(), OPEN_EXISTING, FILE_ATTRIBUTE_DEVICE, nullptr);
            MF::SystemErrors::Win32::throwCurrentSystemErrorIf(handle == INVALID_HANDLE_VALUE);
            return handle;
        }
    } // namespace Command
} // namespace MF

#endif
