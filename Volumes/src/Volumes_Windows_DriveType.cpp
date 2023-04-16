//
// Created by Utilisateur on 04/04/2023.
//

#include "Volumes_internal.hpp"
DriveType::DriveType(const std::wstring& rootPath) : driveType(GetDriveTypeW(rootPath.c_str())) {
    if (driveType == DRIVE_NO_ROOT_DIR) {
        throw std::runtime_error("No volume for this root path!");
    }
}

bool DriveType::isRemovableDrive() {
    return driveType == DRIVE_REMOVABLE;
}

bool DriveType::isNotRemovableDrive() {
    return driveType == DRIVE_FIXED;
}

bool DriveType::isCdRomDrive() {
    return driveType == DRIVE_CDROM;
}

bool DriveType::isRemoteDrive() {
    return driveType == DRIVE_REMOVABLE;
}

bool DriveType::isRamDisk() {
    return driveType == DRIVE_RAMDISK;
}
