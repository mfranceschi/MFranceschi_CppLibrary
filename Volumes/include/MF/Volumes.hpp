//
// Created by MartinF on 05/04/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_VOLUMES_HPP
#define MFRANCESCHI_CPPLIBRARIES_VOLUMES_HPP

#include "MF/Filesystem.hpp"
#include "MF/Optionals.hpp"

namespace MF
{
    namespace Volumes
    {
        using ::MF::Filesystem::Filename_t;
        using ::MF::Filesystem::Filesize_t;
        using ::MF::Optionals::OptionalBool;
        using ::MF::Optionals::OptionalPtr;
#if MF_WINDOWS
        using ::MF::Filesystem::WideFilename_t;
#endif

        struct FileSystem {
            virtual Filename_t getName() = 0;
            virtual bool hasUnicodeSupportForFileNames() = 0;
            virtual bool hasFileBasedCompressionSupport() = 0;
            virtual bool hasCaseSensitiveFileNamesSupport() = 0;
        };

        struct Volume {
            virtual Filename_t getName() = 0;
            virtual Filename_t getFileSystemName() = 0;
            virtual Filename_t getSystemIdentifier() = 0;
            virtual std::vector<Filename_t> getMountPoints() = 0;
            virtual bool isMounted() {
                return !getMountPoints().empty();
            }

#if MF_WINDOWS
            virtual WideFilename_t getNameWide() = 0;
            virtual WideFilename_t getFileSystemNameWide() = 0;
            virtual WideFilename_t getSystemIdentifierWide() = 0;
            virtual std::vector<WideFilename_t> getMountPointsWide() = 0;
#endif

            /** @returns the disk sector size */
            virtual OptionalPtr<Filesize_t> getBytesPerSector() = 0;

            /** @returns the number of sectors in a given allocation unit
             * (also known as cluster, or block). */
            virtual OptionalPtr<std::uint16_t> getSectorsPerAllocationUnit() = 0;

            virtual OptionalPtr<Filesize_t> getTotalSize() = 0;
            virtual OptionalPtr<Filesize_t> getFreeSize() = 0;
            virtual OptionalPtr<Filesize_t> getUsedSize() = 0;

            virtual OptionalBool isRemovableDrive() = 0;
            virtual OptionalBool isNotRemovableDrive() = 0;
            virtual OptionalBool isCdRomDrive() = 0;
            virtual OptionalBool isRemoteDrive() = 0;
            virtual OptionalBool isRamDisk() = 0;

            virtual OptionalBool isReadOnly() = 0;
            virtual OptionalBool hasUnicodeSupportForFileNames() = 0;
            virtual OptionalBool hasFileBasedCompressionSupport() = 0;
            virtual OptionalBool hasCaseSensitiveFileNamesSupport() = 0;

            virtual ~Volume() = default;
        };

        std::vector<std::unique_ptr<Volume>> listAll();

        std::unique_ptr<Volume> getMaybeForMountPoint(const Filename_t& path);

        /**
         * Exception type thrown when calling most methods on a volume that is not mounted.
         */
        struct NotMountedException : std::runtime_error {
            NotMountedException(const std::string& volumeSystemId);
            NotMountedException(const std::wstring& volumeSystemId);
        };
    } // namespace Volumes
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_VOLUMES_HPP
