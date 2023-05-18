//
// Created by MartinF on 05/04/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_VOLUMES_HPP
#define MFRANCESCHI_CPPLIBRARIES_VOLUMES_HPP

#include "MF/Filesystem.hpp"

namespace MF
{
    namespace Volumes
    {
        struct Volume {
            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdiskspaceinformationw

            /** @returns the disk sector size */
            virtual Filesystem::Filesize_t getBytesPerSector() = 0;

            /** @returns the number  */
            virtual std::uint16_t getSectorsPerAllocationUnit() = 0;
            virtual Filesystem::Filesize_t getTotalSize() = 0;
            virtual Filesystem::Filesize_t getFreeSize() = 0;
            virtual Filesystem::Filesize_t getUsedSize() = 0;

            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getdrivetypew
            virtual bool isRemovableDrive() = 0;
            virtual bool isNotRemovableDrive() = 0;
            virtual bool isCdRomDrive() = 0;
            virtual bool isRemoteDrive() = 0;
            virtual bool isRamDisk() = 0;

            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumeinformationa
            virtual Filesystem::Filename_t getName() = 0;
            virtual Filesystem::Filename_t getFileSystemName() = 0;
            virtual bool isReadOnly() = 0;
            virtual bool hasUnicodeSupportForFileNames() = 0;
            virtual bool hasCompressionSupport() = 0;
            virtual bool hasCaseSensitiveFileNamesSupport() = 0;

            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/winioctl/ni-winioctl-fsctl_get_boot_area_info
            // and
            // https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/fsctl-get-boot-area-info
            // and using
            // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_boot_area_info
            virtual std::uint16_t getBootSectorsCount() = 0;

            virtual bool isBootable() {
                return getBootSectorsCount() > 0;
            }

            // From
            // https://learn.microsoft.com/en-us/windows/win32/api/winioctl/ni-winioctl-fsctl_get_compression
            // and using http://winapi.freetechsecrets.com/win32/WIN32FSCTLGETCOMPRESSION.htm
            virtual bool isCompressed() = 0;

            // If I need to create a File Handle to a drive, to know is this:
            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea#physical-disks-and-volumes

            virtual ~Volume() = default;
        };

        std::unique_ptr<Volume> get0();
        std::vector<std::unique_ptr<Volume>> get1();
    } // namespace Volumes
} // namespace MF

#endif // MFRANCESCHI_CPPLIBRARIES_VOLUMES_HPP
