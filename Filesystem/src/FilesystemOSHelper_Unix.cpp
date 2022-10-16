//
// Created by Utilisateur on 22/03/2022.
//

//
// Created by MartinF on 05/03/2022.
//

#if MF_UNIX

#    include <dirent.h>
#    include <fcntl.h>
#    include <unistd.h>

#    include <functional>

#    include "FilesystemOSHelper.hpp"
#    include "MF/SystemErrors.hpp"

using namespace MF::SystemErrors;

namespace MF
{
    namespace Filesystem
    {
        void osReadFileToBuffer(const Filename_t &filename, char *buffer, Filesize_t bufferSize) {
            int fileDescriptor = open(filename.c_str(), O_RDONLY);
            Errno::throwCurrentSystemErrorIf(fileDescriptor == -1);

            ssize_t bytesRead = read(fileDescriptor, buffer, bufferSize);
            if (bytesRead == bufferSize) {
                close(fileDescriptor);
            } else {
                auto currentSystemError = Errno::getCurrentErrorCode();
                close(fileDescriptor);
                throw Errno::getSystemErrorForErrorCode(currentSystemError);
            }
        }

        void osGetDirectoryContents(
            const Filename_t &directoryName, std::vector<Filename_t> &result) {
            static Filename_t CURRENT_FOLDER = ".";
            static Filename_t PARENT_FOLDER = "..";
            std::unique_ptr<DIR, decltype(&closedir)> dirStream(
                opendir(directoryName.c_str()), closedir);
            Errno::throwCurrentSystemErrorIf(dirStream == nullptr);

            dirent *dir_entry = nullptr;
            Filename_t tempFilename;

            Errno::setCurrentErrorCode(0);
            while ((dir_entry = readdir(dirStream.get())) != nullptr) {
                tempFilename = static_cast<const char *>(dir_entry->d_name);
                if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                    continue;
                }

                if (dir_entry->d_type == DT_DIR) {
                    tempFilename.append(FILE_SEPARATOR);
                } else if (dir_entry->d_type == DT_UNKNOWN && isDir(directoryName + tempFilename)) {
                    tempFilename.append(FILE_SEPARATOR);
                }
                result.emplace_back(tempFilename);
                Errno::setCurrentErrorCode(0);
            }

            const auto currentErrorCode = Errno::getCurrentErrorCode();
            if (currentErrorCode != 0) {
                throw Errno::getSystemErrorForErrorCode(currentErrorCode);
            }
        }
    } // namespace Filesystem
} // namespace MF

#endif
