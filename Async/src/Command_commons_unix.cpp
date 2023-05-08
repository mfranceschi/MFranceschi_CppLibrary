//
// Created by MartinF on 08/05/2023.
//

#if MF_UNIX
#    include "Command_commons_unix.hpp"

#    include <fcntl.h>
#    include <unistd.h>

#    include "MF/SystemErrors.hpp"

namespace MF
{
    namespace Command
    {
        using ::MF::SystemErrors::Errno;
        const StreamItem INVALID_STREAM_ITEM = -1;

        static void makePipe(StreamItem &read, StreamItem &write) {
            std::array<int, 2> fileDescriptors{};
            Errno::throwCurrentSystemErrorIf(pipe(fileDescriptors.data()) != 0);
            read = fileDescriptors[0];
            write = fileDescriptors[1];
        }

        static void makeFdClosedOnExec(int fileDescriptor) {
            Errno::throwCurrentSystemErrorIf(fcntl(fileDescriptor, F_SETFD, FD_CLOEXEC) != -1);
        }

        PipeStreams makePipeThatChildWillReadFrom() {
            PipeStreams pipeStreams;
            makePipe(pipeStreams.readFromPipe, pipeStreams.writeToPipe);
            makeFdClosedOnExec(pipeStreams.writeToPipe);
            return pipeStreams;
        }

        PipeStreams makePipeThatChildWillWriteOn() {
            PipeStreams pipeStreams;
            makePipe(pipeStreams.readFromPipe, pipeStreams.writeToPipe);
            makeFdClosedOnExec(pipeStreams.readFromPipe);
            return pipeStreams;
        }

        StreamItem openFileToRead(const Filename_t &filename) {
            StreamItem streamItem = open(filename.c_str(), O_RDONLY);
            Errno::throwCurrentSystemErrorIf(streamItem == -1);
            return streamItem;
        }

        StreamItem openFileToWrite(const Filename_t &filename) {
            StreamItem streamItem = open(filename.c_str(), O_WRONLY);
            Errno::throwCurrentSystemErrorIf(streamItem == -1);
            return streamItem;
        }

        StreamItem openNullFileToWrite() {
            return openFileToWrite("/dev/null");
        }
    } // namespace Command
} // namespace MF
#endif
