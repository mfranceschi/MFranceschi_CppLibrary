//
// Created by MartinF on 15/10/2022.
//

#if MF_WINDOWS

#    include <iostream>
#    include <mutex>

#    include "MF/Commons.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Volumes.hpp"
#    include "Volumes_internal.hpp"

using namespace MF::Volumes;

namespace MF
{
    namespace Volumes
    {
        template <typename T>
        struct ProviderWithSimpleConstructor : MF::Commons::NoCopy, MF::Commons::NoMove {
           public:
            ProviderWithSimpleConstructor() = default;
            ~ProviderWithSimpleConstructor() override = default;

            template <typename... ConstrParams>
            T& get(const ConstrParams&... constrParams) {
                std::call_once(
                    onceFlag,
                    [this](const ConstrParams&... constrParams1) {
                        store = std::make_unique<T>(constrParams1...);
                    },
                    constrParams...);
                return *store;
            }

           private:
            std::once_flag onceFlag{};
            std::unique_ptr<T> store;
        };

        struct VolumeInfo_Windows : VolumeInformation {
            VolumeInfo_Windows(const std::wstring& rootPath) : rootPath(rootPath) {
            }

           private:
            const std::wstring rootPath;

           public:
            Filesystem::Filesize_t getBytesPerSector() override {
                return diskSpaceInfo.get(rootPath).getBytesPerSector();
            }

            Filesystem::Filesize_t getTotalSize() override {
                return diskSpaceInfo.get(rootPath).getTotalSize();
            }

            Filesystem::Filesize_t getFreeSize() override {
                return diskSpaceInfo.get(rootPath).getFreeSize();
            }

            Filesystem::Filesize_t getUsedSize() override {
                return diskSpaceInfo.get(rootPath).getUsedSize();
            }

            bool isRemovableDrive() override {
                return driveType.get(rootPath).isRemovableDrive();
            }

            bool isNotRemovableDrive() override {
                return driveType.get(rootPath).isNotRemovableDrive();
            }

            bool isCdRomDrive() override {
                return driveType.get(rootPath).isCdRomDrive();
            }

            bool isRemoteDrive() override {
                return driveType.get(rootPath).isRemoteDrive();
            }

            bool isRamDisk() override {
                return driveType.get(rootPath).isRamDisk();
            }

            Filesystem::Filename_t getName() override {
                return volumeInformation.get(rootPath).getName();
            }

            Filesystem::Filename_t getFileSystemName() override {
                return volumeInformation.get(rootPath).getFileSystemName();
            }

            bool isReadOnly() override {
                return volumeInformation.get(rootPath).isReadOnly();
            }

            bool hasUnicodeSupportForFileNames() override {
                return volumeInformation.get(rootPath).hasUnicodeSupportForFileNames();
            }

            bool hasCompressionSupport() override {
                return volumeInformation.get(rootPath).hasCompressionSupport();
            }

            bool hasCaseSensitiveFileNamesSupport() override {
                return volumeInformation.get(rootPath).hasCaseSensitiveFileNamesSupport();
            }

            uint16_t getBootSectorsCount() override {
                return 0;
            }
            bool isCompressed() override {
                return false;
            }

           private:
            ProviderWithSimpleConstructor<DiskSpaceInfo> diskSpaceInfo;
            ProviderWithSimpleConstructor<DriveType> driveType;
            ProviderWithSimpleConstructor<::VolumeInformation> volumeInformation;
        };

        std::unique_ptr<VolumeInformation> get0() {
            auto rootPath0 = getPathsForVolumeGuid(enumerateVolumeGuids()[0])[0];
            std::wcout << rootPath0 << std::endl;
            return std::make_unique<VolumeInfo_Windows>(rootPath0);
        }
    } // namespace Volumes
} // namespace MF
#endif
