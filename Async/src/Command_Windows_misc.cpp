//
// Created by MartinF on 10/04/2023.
//

#if MF_WINDOWS
#    include <tchar.h>

#    include <cassert>

#    include "Command_Windows_commons.hpp"

namespace MF
{
    namespace Command
    {
        /// Use this handle as a sample for inheritable handles.
        static SECURITY_ATTRIBUTES securityAttributesForInheritableHandles{
            sizeof(SECURITY_ATTRIBUTES), nullptr, true};

        SECURITY_ATTRIBUTES &getInheritableSecAttr() {
            return securityAttributesForInheritableHandles;
        }

        std::vector<TCHAR> makeCommandLine(
            const Filename_t *executable, const std::vector<Filename_t> *arguments) {
            std::basic_ostringstream<TCHAR> osStream;

            // TODO escape whitespaces to `\ `
            osStream << executable->c_str();
            if (!arguments->empty()) {
                osStream << " ";
                for (const auto &arg : *arguments) {
                    osStream << arg << " ";
                }
            }
            auto s = osStream.str();
            std::vector<TCHAR> result;
            result.reserve(s.size() + 1);
            assert(!_tcsncpy_s(result.data(), result.capacity(), s.c_str(), s.length()));
            return result;
        }

        void populateStartupInfo(const CommandCall &commandCall, STARTUPINFO &startupinfo) {
            ZeroMemory(&startupinfo, sizeof(startupinfo));
            startupinfo.cb = sizeof(startupinfo);
            startupinfo.dwFlags |= STARTF_USESTDHANDLES;
            startupinfo.hStdOutput =
                dynamic_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdOutChoice))
                    .getStreamItem(OutputStream_e::StdOut);
            startupinfo.hStdError =
                dynamic_cast<ConsoleOutputChoice_Windows &>(*(commandCall.stdOutChoice))
                    .getStreamItem(OutputStream_e::StdErr);
            startupinfo.hStdInput =
                dynamic_cast<ConsoleInputChoice_Windows &>(*(commandCall.stdInChoice))
                    .getStreamItem();
        }

        void makeHandleInheritable(HANDLE handle, bool yesOrNo) {
            assert(SetHandleInformation(
                handle, HANDLE_FLAG_INHERIT, yesOrNo ? HANDLE_FLAG_INHERIT : 0));
        }
    } // namespace Command
} // namespace MF

#endif
