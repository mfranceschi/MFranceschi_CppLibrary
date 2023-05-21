//
// Created by Utilisateur on 22/05/2023.
//

#include "MF/Strings.hpp"
#include "MF/Volumes.hpp"

namespace MF
{
    namespace Volumes
    {
        NotMountedException::NotMountedException(const std::string& volumeSystemId)
            : std::runtime_error(
                  "Volume with system identifier " + volumeSystemId + " is not mounted!") {
        }

        NotMountedException::NotMountedException(const std::wstring& volumeSystemId)
            : std::runtime_error(
                  "Volume with system identifier " +
                  MF::Strings::Conversions::wideCharToUtf8(volumeSystemId) + " is not mounted!") {
        }
    } // namespace Volumes
} // namespace MF
