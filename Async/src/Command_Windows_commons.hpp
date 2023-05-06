//
// Created by MartinF on 10/04/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMAND_WINDOWS_COMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMAND_WINDOWS_COMMONS_HPP

#if MF_WINDOWS
#    include "MF/Command.hpp"
#    include "MF/Commons.hpp"
#    include "MF/LightWindows.hpp"

namespace MF
{
    namespace Command
    {
        using StreamItem = HANDLE;
        extern const StreamItem INVALID_STREAM_ITEM;

        using ProcessItem = HANDLE;

        enum class OutputStream_e { StdOut, StdErr };

        struct CommandComponent : MF::Commons::NoCopy, MF::Commons::NoMove {
            virtual void beforeStart() {
            }

            virtual void afterStart() {
            }

            virtual void afterStop() {
            }

            virtual ~CommandComponent() = default;
        };

        struct ConsoleOutputChoice {
            virtual ~ConsoleOutputChoice() = default;
        };

        struct ConsoleInputChoice {
            virtual ~ConsoleInputChoice() = default;
        };

        struct PipeStreams {
            StreamItem writeToPipe = INVALID_STREAM_ITEM;
            StreamItem readFromPipe = INVALID_STREAM_ITEM;
        };

        PipeStreams makePipeThatChildWillReadFrom();
        PipeStreams makePipeThatChildWillWriteOn();

        StreamItem openFileToRead(const Filename_t &filename);
        StreamItem openFileToWrite(const Filename_t &filename);
        StreamItem openNullFileToWrite();

#    if MF_WINDOWS
        StreamItem openFileToRead(const WideFilename_t &filename);
        StreamItem openFileToWrite(const WideFilename_t &filename);

        struct ConsoleOutputChoice_Windows : ConsoleOutputChoice, CommandComponent {
            virtual StreamItem getStreamItem(OutputStream_e) const = 0;
        };

        struct ConsoleInputChoice_Windows : ConsoleInputChoice, CommandComponent {
            virtual StreamItem getStreamItem() const = 0;
        };

        std::vector<char> makeCommandLine(
            const Filename_t *executable, const std::vector<std::string> *arguments);
        std::vector<wchar_t> makeCommandLine(
            const WideFilename_t *executable, const std::vector<std::wstring> *arguments);

        void populateStartupInfo(const CommandCall &commandCall, STARTUPINFOA &startupinfo);
        void populateStartupInfo(const WideCommandCall &commandCall, STARTUPINFOW &startupinfo);

        SECURITY_ATTRIBUTES &getInheritableSecAttr();

        void closeH(HANDLE &handle);

#    endif

    } // namespace Command
} // namespace MF

#endif

#endif // MFRANCESCHI_CPPLIBRARIES_COMMAND_WINDOWS_COMMONS_HPP
