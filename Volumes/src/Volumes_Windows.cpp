//
// Created by MartinF on 15/10/2022.
//

#if MF_WINDOWS

#    include <iostream>

#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Volumes.hpp"
#    include "Volumes_internal.hpp"

using namespace MF::Volumes;

namespace MF
{
    namespace Volumes
    {

        struct VolumeInfo_Windows : VolumeInformation {
            VolumeInfo_Windows(const std::wstring& rootPath) : rootPath(rootPath) {
            }

            const std::wstring rootPath;

            Filesystem::Filesize_t getBytesPerSector() override {
                return getDiskSpaceInfoWrapper().getBytesPerSector();
            }

            Filesystem::Filesize_t getTotalSize() override {
                return getDiskSpaceInfoWrapper().getTotalSize();
            }

            Filesystem::Filesize_t getFreeSize() override {
                return getDiskSpaceInfoWrapper().getFreeSize();
            }

            Filesystem::Filesize_t getUsedSize() override {
                return getDiskSpaceInfoWrapper().getUsedSize();
            }

            bool isRemovableDrive() override {
                return getDriveType().isRemovableDrive();
            }

            bool isNotRemovableDrive() override {
                return getDriveType().isNotRemovableDrive();
            }

            bool isCdRomDrive() override {
                return getDriveType().isCdRomDrive();
            }

            bool isRemoteDrive() override {
                return getDriveType().isRemoteDrive();
            }

            bool isRamDisk() override {
                return getDriveType().isRamDisk();
            }

            Filesystem::Filename_t getName() override {
                return MF::Filesystem::Filename_t();
            }
            Filesystem::Filename_t getFileSystemName() override {
                return MF::Filesystem::Filename_t();
            }
            bool isReadOnly() override {
                return false;
            }
            bool hasUnicodeSupportForFileNames() override {
                return false;
            }
            bool hasCompressionSupport() override {
                return false;
            }
            bool hasCaseSensitiveFileNamesSupport() override {
                return false;
            }
            uint16_t getBootSectorsCount() override {
                return 0;
            }
            bool isCompressed() override {
                return false;
            }

           private:
            std::unique_ptr<DiskSpaceInfo> diskSpaceInfo;

            DiskSpaceInfo& getDiskSpaceInfoWrapper() {
                if (!diskSpaceInfo) {
                    diskSpaceInfo = std::make_unique<DiskSpaceInfo>(rootPath);
                }
                return *diskSpaceInfo;
            }

            std::unique_ptr<DriveType> driveType;

            DriveType& getDriveType() {
                if (!driveType) {
                    driveType = std::make_unique<DriveType>(rootPath);
                }
                return *driveType;
            }
        };

        std::unique_ptr<VolumeInformation> get0() {
            auto rootPath0 = getPathsForVolumeGuid(enumerateVolumeGuids()[0])[0];
            std::wcout << rootPath0 << std::endl;
            return std::make_unique<VolumeInfo_Windows>(rootPath0);
        }
    } // namespace Volumes
} // namespace MF
#endif
