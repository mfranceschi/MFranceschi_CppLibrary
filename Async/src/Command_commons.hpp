//
// Created by MartinF on 10/04/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_HPP

#include "MF/Command.hpp"
#include "MF/Commons.hpp"

#if MF_WINDOWS
#    include "MF/LightWindows.hpp"
#endif

namespace MF
{
    namespace Command
    {
#if MF_WINDOWS
        using StreamItem = HANDLE;
        using ProcessItem = HANDLE;
#endif

        extern const StreamItem INVALID_STREAM_ITEM;

        struct CommandComponent : MF::Commons::NoCopy, MF::Commons::NoMove {
            virtual void beforeStart() {
            }

            virtual void afterStart() {
            }

            virtual void afterStop() {
            }

            virtual ~CommandComponent() = default;
        };

        struct ConsoleOutputChoice : CommandComponent {
            virtual StreamItem getStreamItemForStdOut() const = 0;
            virtual StreamItem getStreamItemForStdErr() const {
                return getStreamItemForStdOut();
            }
        };

        struct ConsoleInputChoice : CommandComponent {
            virtual StreamItem getStreamItemForStdIn() const = 0;
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
    } // namespace Command
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_COMMAND_COMMONS_HPP
