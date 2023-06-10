//
// Created by Utilisateur on 04/04/2023.
//

#include <vector>

#include "MF/LightWindows.hpp"
#include "MF/SystemErrors.hpp"

std::vector<std::wstring> getPathsForVolumeGuid(const std::wstring& guid) {
    // Adapted from https://learn.microsoft.com/en-us/windows/win32/fileio/displaying-volume-paths

    DWORD charCount = MAX_PATH + 1;
    std::vector<wchar_t> names;

    for (;;) {
        names.reserve(charCount);

        if (GetVolumePathNamesForVolumeNameW(guid.c_str(), names.data(), charCount, &charCount) ==
            TRUE) {
            // We have all the paths that we need, we can continue.
            break;
        }

        auto lastError = MF::SystemErrors::Win32::getCurrentErrorCode();
        if (lastError != ERROR_MORE_DATA) {
            throw MF::SystemErrors::Win32::getSystemErrorForErrorCode(lastError);
        }
    }

    std::vector<std::wstring> result;
    for (const wchar_t* currentPath = names.data(); currentPath[0] != L'\0';
         currentPath += wcslen(currentPath) + 1) {
        result.push_back(currentPath);
    }

    return result;
}
