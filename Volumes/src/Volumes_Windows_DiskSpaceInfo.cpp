//
// Created by Utilisateur on 04/04/2023.
//

#if MF_WINDOWS
#    include "MF/SystemErrors.hpp"
#    include "Volumes_internal.hpp"

DiskSpaceInfo::DiskSpaceInfo(const std::wstring& rootPath) {
    MF::SystemErrors::Win32::throwCurrentSystemErrorIf(
        GetDiskSpaceInformationW(rootPath.data(), &dsi));
}

Filesize_t DiskSpaceInfo::getBytesPerSector() {
    return dsi.BytesPerSector;
}

Filesize_t DiskSpaceInfo::getTotalSize() {
    return dsi.ActualTotalAllocationUnits * dsi.SectorsPerAllocationUnit * dsi.BytesPerSector;
}

Filesize_t DiskSpaceInfo::getFreeSize() {
    return dsi.ActualAvailableAllocationUnits * dsi.SectorsPerAllocationUnit * dsi.BytesPerSector;
}

Filesize_t DiskSpaceInfo::getUsedSize() {
    return dsi.UsedAllocationUnits * dsi.SectorsPerAllocationUnit * dsi.BytesPerSector;
}
#endif
