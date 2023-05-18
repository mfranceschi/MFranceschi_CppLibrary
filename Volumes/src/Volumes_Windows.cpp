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
        static const std::wstring EMPTY;

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

        struct VolumeInfo_Windows : Volume {
            VolumeInfo_Windows(const std::wstring& volumeGuid)
                : volumeGuid(volumeGuid),
                  paths(getPathsForVolumeGuid(volumeGuid)),
                  rootPath(paths.empty() ? EMPTY : paths[0]) {
            }

           private:
            const std::wstring volumeGuid;
            const std::vector<std::wstring> paths;
            const std::wstring& rootPath;

           public:
            Filesystem::Filesize_t getBytesPerSector() override {
                return diskSpaceInfo.get(rootPath).getBytesPerSector();
            }

            std::uint16_t getSectorsPerAllocationUnit() override {
                return diskSpaceInfo.get(rootPath).getSectorsPerAllocationUnit();
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
                return bootSectorsInfo.get(rootPath).getBootSectorsCount();
            }

            bool isCompressed() override {
                return false;
            }

           private:
            ProviderWithSimpleConstructor<GetDiskSpaceInfo_Windows> diskSpaceInfo;
            ProviderWithSimpleConstructor<GetDriveType_Windows> driveType;
            ProviderWithSimpleConstructor<GetVolumeInformation_Windows> volumeInformation;
            ProviderWithSimpleConstructor<IoControl_GetBootSectorsInfo> bootSectorsInfo;
        };

        std::unique_ptr<Volume> get0() {
            auto rootPath0 = getPathsForVolumeGuid(enumerateVolumeGuids()[0])[0];
            std::wcout << rootPath0 << std::endl;
            return std::make_unique<VolumeInfo_Windows>(rootPath0);
        }

        std::vector<std::unique_ptr<Volume>> get1() {
            const std::vector<std::wstring> volumeGuids = enumerateVolumeGuids();
            std::vector<std::unique_ptr<Volume>> volumes;
            volumes.reserve(volumeGuids.size());
            for (const std::wstring& guid : volumeGuids) {
                volumes.push_back(std::make_unique<VolumeInfo_Windows>(guid));
            }
            return volumes;
        }
    } // namespace Volumes
} // namespace MF
#endif
