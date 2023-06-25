//
// Created by Utilisateur on 25/06/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_NOTMOUNTEDVOLUME_HPP
#define MFRANCESCHI_CPPLIBRARIES_NOTMOUNTEDVOLUME_HPP

//
// Created by MartinF on 15/10/2022.
//

#include <cassert>
#include <iostream>
#include <mutex>

#include "MF/Strings.hpp"
#include "MF/Volumes.hpp"

using namespace MF::Volumes;

namespace MF
{
    namespace Volumes
    {
        struct NotMountedVolume : Volume {
           public:
            bool isMounted() override {
                return false;
            }

            OptionalPtr<Filesize_t> getBytesPerSector() override {
                return Optionals::empty<Filesize_t>();
            }

            OptionalPtr<std::uint16_t> getSectorsPerAllocationUnit() override {
                return Optionals::empty<std::uint16_t>();
            }

            OptionalPtr<Filesize_t> getTotalSize() override {
                return Optionals::empty<Filesize_t>();
            }

            OptionalPtr<Filesize_t> getFreeSize() override {
                return Optionals::empty<Filesize_t>();
            }

            OptionalPtr<Filesize_t> getUsedSize() override {
                return Optionals::empty<Filesize_t>();
            }

            OptionalBool isRemovableDrive() override {
                return OptionalBool::empty();
            }

            OptionalBool isNotRemovableDrive() override {
                return OptionalBool::empty();
            }

            OptionalBool isCdRomDrive() override {
                return OptionalBool::empty();
            }

            OptionalBool isRemoteDrive() override {
                return OptionalBool::empty();
            }

            OptionalBool isRamDisk() override {
                return OptionalBool::empty();
            }

            Filename_t getName() override {
                throwNotMounted();
            }

            Filename_t getFileSystemName() override {
                throwNotMounted();
            }

            std::vector<Filename_t> getMountPoints() override {
                return {};
            }

#if MF_WINDOWS
            WideFilename_t getNameWide() override {
                throwNotMounted();
            }

            WideFilename_t getFileSystemNameWide() override {
                throwNotMounted();
            }

            std::vector<WideFilename_t> getMountPointsWide() override {
                return {};
            }
#endif

            OptionalBool isReadOnly() override {
                return OptionalBool::empty();
            }

            OptionalBool hasUnicodeSupportForFileNames() override {
                return OptionalBool::empty();
            }

            OptionalBool hasFileBasedCompressionSupport() override {
                return OptionalBool::empty();
            }

            OptionalBool hasCaseSensitiveFileNamesSupport() override {
                return OptionalBool::empty();
            }

#if MF_WINDOWS
            NotMountedVolume(const std::wstring& volumeGuid) : volumeGuid(volumeGuid) {
            }

            Filename_t getSystemIdentifier() override {
                return MF::Strings::Conversions::wideCharToUtf8(volumeGuid);
            }

            WideFilename_t getSystemIdentifierWide() override {
                return volumeGuid;
            }

           private:
            const std::wstring volumeGuid;

            [[noreturn]] void throwNotMounted() {
                throw NotMountedException(volumeGuid);
            }
#endif
        };
    } // namespace Volumes
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_NOTMOUNTEDVOLUME_HPP
