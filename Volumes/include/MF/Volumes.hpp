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

            /*
#if MF_WINDOWS
            virtual WideFilename_t getNameWide() = 0;
            virtual Filename_t getFileSystemName() = 0;
            virtual Filename_t getSystemIdentifier() = 0;
            virtual std::vector<Filename_t> getMountPoints() = 0;
#endif
             */

            /** @returns the disk sector size */
            virtual OptionalPtr<Filesize_t> getBytesPerSector() = 0;

            /** @returns the number of sectors in a given allocation unit
             * (also known as cluster, or block). */
            virtual OptionalPtr<std::uint16_t> getSectorsPerAllocationUnit() = 0;

            virtual OptionalPtr<Filesize_t> getTotalSize() = 0;
            virtual OptionalPtr<Filesize_t> getFreeSize() = 0;
            virtual OptionalPtr<Filesize_t> getUsedSize() = 0;

            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdrivetypew
            virtual OptionalBool isRemovableDrive() = 0;
            virtual OptionalBool isNotRemovableDrive() = 0;
            virtual OptionalBool isCdRomDrive() = 0;
            virtual OptionalBool isRemoteDrive() = 0;
            virtual OptionalBool isRamDisk() = 0;

            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumeinformationa
            virtual OptionalBool isReadOnly() = 0;
            virtual OptionalBool hasUnicodeSupportForFileNames() = 0;
            virtual OptionalBool hasFileBasedCompressionSupport() = 0;
            virtual OptionalBool hasCaseSensitiveFileNamesSupport() = 0;

            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/winioctl/ni-winioctl-fsctl_get_boot_area_info
            // and
            // https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/fsctl-get-boot-area-info
            // and using
            // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_boot_area_info
            // virtual std::uint16_t getBootSectorsCount() = 0; // TODO
            // virtual bool isBootable() {
            //   return getBootSectorsCount() > 0;
            // }

            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/winioctl/ni-winioctl-fsctl_get_compression
            // and using http://winapi.freetechsecrets.com/win32/WIN32FSCTLGETCOMPRESSION.htm
            // virtual bool isCompressed() = 0;

            // If I need to create a File Handle to a drive, to know is this:
            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea#physical-disks-and-volumes

            virtual ~Volume() = default;
        };

        std::vector<std::unique_ptr<Volume>> listAll();

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
