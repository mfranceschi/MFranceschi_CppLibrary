//
// Created by Utilisateur on 04/04/2023.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_VOLUMES_INTERNAL_HPP
#define MFRANCESCHI_CPPLIBRARIES_VOLUMES_INTERNAL_HPP

#include <string>
#include <vector>

#include "MF/Filesystem.hpp"
using namespace MF::Filesystem;

#if MF_WINDOWS
#    include "MF/LightWindows.hpp"
std::vector<std::wstring> enumerateVolumeGuids();

/**
 * Returns an array of root paths for that volume, often of format "C:\".
 */
std::vector<std::wstring> getPathsForVolumeGuid(const std::wstring& guid);

struct DiskSpaceInfo {
    DiskSpaceInfo(const std::wstring& rootPath);

    Filesize_t getBytesPerSector();
    Filesize_t getTotalSize();
    Filesize_t getFreeSize();
    Filesize_t getUsedSize();

   private:
    DISK_SPACE_INFORMATION dsi;
};

struct DriveType {
    DriveType(const std::wstring& rootPath);

    bool isRemovableDrive();
    bool isNotRemovableDrive();
    bool isCdRomDrive();
    bool isRemoteDrive();
    bool isRamDisk();

   private:
    UINT driveType = DRIVE_UNKNOWN;
};

#endif

#endif // MFRANCESCHI_CPPLIBRARIES_VOLUMES_INTERNAL_HPP
