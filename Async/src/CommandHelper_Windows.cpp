//
// Created by mfran on 28/04/2020.
//

#if MF_WINDOWS

#    include "CommandHelper.hpp"
#    include "MF/Filesystem.hpp"
#    include "MF/StringSafePlaceHolder.hpp"
#    include "MF/Windows.hpp"
#    include "WindowsAPIHelper.hpp"

namespace MF
{
    namespace Command
    {

        /// Use this handle as a sample for inheritable handles.
        static SECURITY_ATTRIBUTES securityAttributesForInheritableHandles{
            sizeof(SECURITY_ATTRIBUTES), nullptr, true};

        static constexpr unsigned int BUFFER_LENGTH = 4096;

        // ///////////////////////////////////////////////////////////////
        // /////////////////////// INPUT STREAMS /////////////////////////
        // ///////////////////////////////////////////////////////////////

        StreamItem ProcessInputStream_None::getStreamItem() const {
            return GetStdHandle(STD_INPUT_HANDLE);
        }

        void ProcessInputStream_String::beforeStart() {
            SECURITY_ATTRIBUTES securityAttributes{sizeof(SECURITY_ATTRIBUTES), nullptr, true};
            CreatePipe(&readStream, &writeToStream, &securityAttributes, BUFFER_LENGTH);
            MF::Windows::MakeHandleInheritable(readStream);
        }

        void ProcessInputStream_String::afterStart() {
            DWORD lpWritten;
            std::string temp;
            temp.reserve(999);
            for (std::size_t i = 0; i < inputString.length(); i += 999) {
                temp = inputString.substr(i, 999);
                WriteFile(writeToStream, temp.c_str(), 999, &lpWritten, nullptr);
            }
        }

        void ProcessInputStream_String::afterStop() {
            CloseHandle(writeToStream);
            CloseHandle(readStream);
        }

        StreamItem ProcessInputStream_String::getStreamItem() const {
            return readStream;
        }

        void ProcessInputStream_FromFile::beforeStart() {
            fileStream = CreateFile(
                filename.c_str(), FILE_GENERIC_READ, FILE_SHARE_READ,
                &securityAttributesForInheritableHandles, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                nullptr);
        }

        void ProcessInputStream_FromFile::afterStop() {
            CloseHandle(fileStream);
        }

        StreamItem ProcessInputStream_FromFile::getStreamItem() const {
            return fileStream;
        }

        // ///////////////////////////////////////////////////////////////
        // ////////////////////// OUTPUT STREAMS /////////////////////////
        // ///////////////////////////////////////////////////////////////

        StreamItem ProcessOutputStream_Keep::getStreamItem() const {
            return GetStdHandle(STD_OUTPUT_HANDLE);
        }

        StreamItem ProcessErrorStream_Keep::getStreamItem() const {
            return GetStdHandle(STD_ERROR_HANDLE);
        }

        const MF::Filesystem::Filename_t ProcessOutputStream_Kill::KILL_FILENAME =
            MAKE_FILE_NAME "NUL";

        void ProcessOutputStream_Kill::beforeStart() {
            fileStream = CreateFile(
                KILL_FILENAME, // weirdly it does not work otherwise
                FILE_GENERIC_WRITE, FILE_SHARE_READ, &securityAttributesForInheritableHandles,
                OPEN_ALWAYS, FILE_ATTRIBUTE_DEVICE, nullptr);
        }

        void ProcessOutputStream_Export::beforeStart() {
            fileStream = CreateFile(
                filename.c_str(), FILE_GENERIC_WRITE, FILE_SHARE_READ,
                &securityAttributesForInheritableHandles, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                nullptr);

            if (APPEND) {
                SetFilePointer(fileStream, 0, nullptr, FILE_END);
            }
        }

        void ProcessOutputStream_Export::afterStop() {
            CloseHandle(fileStream);
        }

        StreamItem ProcessOutputStream_Export::getStreamItem() const {
            return fileStream;
        }

        void ProcessOutputStream_Retrieve::beforeStart() {
            DWORD dwMode = PIPE_NOWAIT;
            SECURITY_ATTRIBUTES securityAttributes{sizeof(SECURITY_ATTRIBUTES), nullptr, true};
            CreatePipe(&readStream, &writeStream, &securityAttributes, BUFFER_LENGTH);
            MF::Windows::MakeHandleInheritable(writeStream);
            SetNamedPipeHandleState(readStream, &dwMode, nullptr, nullptr);
        }

        void ProcessOutputStream_Retrieve::beforeStop() {
            DWORD dwRead;
            CHAR chBuf[BUFFER_LENGTH];

            if (ReadFile(readStream, chBuf, BUFFER_LENGTH - 1, &dwRead, nullptr) || (dwRead != 0)) {
                chBuf[dwRead] = '\0';
                oss << chBuf;
            }
        }

        void ProcessOutputStream_Retrieve::afterStop() {
            DWORD dwRead;
            CHAR chBuf[BUFFER_LENGTH];

            while (ReadFile(readStream, chBuf, BUFFER_LENGTH - 1, &dwRead, nullptr) ||
                   (dwRead != 0)) {
                chBuf[dwRead] = '\0';
                oss << chBuf;
            }
            CloseHandle(writeStream);
            CloseHandle(readStream);
        }

        std::string ProcessOutputStream_Retrieve::retrieveOutput() {
            return oss.str();
        }

        StreamItem ProcessOutputStream_Retrieve::getStreamItem() const {
            return writeStream;
        }

        // ///////////////////////////////////////////////////////////////
        // ////////////////////// COMMAND RUNNER /////////////////////////
        // ///////////////////////////////////////////////////////////////

        void CommandRunner::internalStart() {
            // Define all parameters required by the CreateProcess function.
            LPCTCH lpApplicationName = nullptr;
            TCHAR *lpCommandLine;
            {
                // Fill the "Command line" string
                std::basic_ostringstream<std::remove_pointer_t<decltype(executable)>::value_type>
                    osStream;

                osStream << executable->c_str();
                if (!arguments->empty()) {
                    osStream << " ";
                    for (const auto &arg : *arguments) {
                        osStream << arg << " ";
                    }
                }
                auto streamOutput = osStream.str();

                const TCHAR *commandStringPointer = streamOutput.c_str();
                lpCommandLine = new TCHAR[streamOutput.size() + 1];
                auto copyResult =
                    StringCchCopy(lpCommandLine, streamOutput.size() + 1, commandStringPointer);

                if (FAILED(copyResult)) {
                    if (copyResult == STRSAFE_E_INVALID_PARAMETER) {
                        MF::Windows::ShowErrorMessage("StringCchCopy <invalid param>");
                    } else {
                        MF::Windows::ShowErrorMessage("StringCchCopy <insufficient buffer>");
                    }
                }
            }
            LPSECURITY_ATTRIBUTES lpProcessAttributes = nullptr; // No specific security attribute
            LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr; // No specific security attribute
            bool bInheritHandles = true; // Handles are inherited
            DWORD dwCreationFlags = 0; // Creation flags
            LPVOID lpEnvironment = nullptr; // We use the parent's environment
            const TCHAR *lpCurrentDirectory =
                nullptr; // We use the parent's current working directory
            STARTUPINFO startupinfo;
            PROCESS_INFORMATION processInformation;
            LPSTARTUPINFO lpStartupInfo = &startupinfo;
            LPPROCESS_INFORMATION lpProcessInformation = &processInformation;
            {
                ZeroMemory(&startupinfo, sizeof(startupinfo));
                startupinfo.cb = sizeof(startupinfo);
                startupinfo.dwFlags |= STARTF_USESTDHANDLES;
                startupinfo.hStdOutput = processOutputStream->getStreamItem();
                startupinfo.hStdError = processErrorStream->getStreamItem();
                startupinfo.hStdInput = processInputStream->getStreamItem();
                ZeroMemory(&processInformation, sizeof(processInformation));
                // By default, we have nothing in these structs
            }

            // TODO Use the right handles

            bool createProcessResult = CreateProcess(
                lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
                lpProcessInformation);
            if (!createProcessResult) {
                MF::Windows::ShowErrorMessage("CreateProcess");
                // TODO remove later
            }

            childProcessItem = processInformation.hProcess;
            CloseHandle(processInformation.hThread);
        }

        void CommandRunner::internalStop() {
            Windows_WaitForProcess(childProcessItem);
        }

        int CommandRunner::internalGetStatusCode() {
            return Windows_GetExitCodeCommand(childProcessItem);
        }

        void CommandRunner::internalOSCleanUp() {
            CloseHandle(childProcessItem);
        }
    } // namespace Command
} // namespace MF
#endif
