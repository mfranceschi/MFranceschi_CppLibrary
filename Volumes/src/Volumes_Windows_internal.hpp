//
// Created by Utilisateur on 04/04/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_VOLUMES_WINDOWS_INTERNAL_HPP
#define MFRANCESCHI_CPPLIBRARIES_VOLUMES_WINDOWS_INTERNAL_HPP

#if MF_WINDOWS
#    include <string>
#    include <vector>

#    include "MF/LightWindows.hpp"
#    include "MF/Strings.hpp"
#    include "MF/Volumes.hpp"

std::vector<std::wstring> enumerateVolumeGuids();

std::vector<std::wstring> getPathsForVolumeGuid(const std::wstring& guid);

struct GetDiskSpaceInfo_Windows {
    GetDiskSpaceInfo_Windows(const std::wstring& rootPath) {
        // NOTE: while the official documentation mentions booleans,
        // the actual returned value is NOT a boolean but a HRESULT.
        MF::SystemErrors::Win32::setCurrentErrorCode(0);
        const HRESULT hresult = GetDiskSpaceInformationW(rootPath.data(), &dsi);
        MF::SystemErrors::Win32::getCurrentErrorCode();
        MF::SystemErrors::Win32::throwCurrentSystemErrorIf(FAILED(hresult));
        sizeCoeff = dsi.SectorsPerAllocationUnit * dsi.BytesPerSector;
    }

    Filesize_t getBytesPerSector() const {
        return dsi.BytesPerSector;
    }

    std::uint16_t getSectorsPerAllocationUnit() const {
        return dsi.SectorsPerAllocationUnit;
    }

    Filesize_t getTotalSize() const {
        return dsi.ActualTotalAllocationUnits * sizeCoeff;
    }

    Filesize_t getFreeSize() const {
        return dsi.ActualAvailableAllocationUnits * sizeCoeff;
    }

    Filesize_t getUsedSize() const {
        return (dsi.UsedAllocationUnits + dsi.TotalReservedAllocationUnits) * sizeCoeff;
    }

   private:
    DISK_SPACE_INFORMATION dsi{0};
    Filesize_t sizeCoeff{0};
};

struct GetDriveType_Windows {
    GetDriveType_Windows(const std::wstring& rootPath)
        : driveType(GetDriveTypeW(rootPath.c_str())) {
        if (driveType == DRIVE_NO_ROOT_DIR) {
            throw std::runtime_error("No volume for this root path!");
        }
    }

    bool isRemovableDrive() const {
        return driveType == DRIVE_REMOVABLE;
    }

    bool isNotRemovableDrive() const {
        return driveType == DRIVE_FIXED;
    }

    bool isCdRomDrive() const {
        return driveType == DRIVE_CDROM;
    }

    bool isRemoteDrive() const {
        return driveType == DRIVE_REMOTE;
    }

    bool isRamDisk() const {
        return driveType == DRIVE_RAMDISK;
    }

   private:
    UINT driveType = DRIVE_UNKNOWN;
};

struct GetVolumeInformation_Windows {
    GetVolumeInformation_Windows(const std::wstring& rootPath) {
        std::vector<wchar_t> volumeNameBuffer(MAX_PATH + 1);
        std::vector<wchar_t> fileSystemBuffer(MAX_PATH + 1);
        const bool success =
            GetVolumeInformationW(
                rootPath.c_str(), volumeNameBuffer.data(), volumeNameBuffer.capacity(),
                &serialNumber, &maxComponentLength, &flags, fileSystemBuffer.data(),
                fileSystemBuffer.capacity()) == TRUE;
        MF::SystemErrors::Win32::throwCurrentSystemErrorIf(!success);
        volumeName.assign(volumeNameBuffer.data());
        fileSystem.assign(fileSystemBuffer.data());
    }

    Filename_t getName() const {
        return MF::Strings::Conversions::wideCharToUtf8(volumeName);
    }

    WideFilename_t getNameWide() const {
        return volumeName;
    }

    Filename_t getFileSystemName() const {
        return MF::Strings::Conversions::wideCharToUtf8(fileSystem);
    }

    WideFilename_t getFileSystemNameWide() const {
        return fileSystem;
    }

    bool isReadOnly() const {
        return flags & FILE_READ_ONLY_VOLUME;
    }

    bool hasUnicodeSupportForFileNames() const {
        return flags & FILE_UNICODE_ON_DISK;
    }

    bool hasCompressionSupport() const {
        return flags & FILE_FILE_COMPRESSION;
    }

    bool hasCaseSensitiveFileNamesSupport() const {
        return flags & FILE_CASE_SENSITIVE_SEARCH;
    }

   private:
    std::wstring volumeName;
    std::wstring fileSystem;
    DWORD serialNumber{0};
    DWORD maxComponentLength{0};
    DWORD flags{0};
};

#endif

#endif // MFRANCESCHI_CPPLIBRARIES_VOLUMES_WINDOWS_INTERNAL_HPP
