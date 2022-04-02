//
// Created by Utilisateur on 02/04/2022.
//

#include "MF/LightWindows.hpp"
#include "MF/Windows.hpp"

namespace MF
{
    namespace Windows
    {
        FileAttributes::FileAttributes() : FileAttributes(INVALID_FILE_ATTRIBUTES) {
        }

        FileAttributes::FileAttributes(Type value) : dWord(value) {
        }

        FileAttributes::Type FileAttributes::get() const {
            return dWord;
        }

        FileAttributes& FileAttributes::set(Type value) {
            dWord = value;
            return *this;
        }

        bool FileAttributes::isInvalid() const {
            return dWord == INVALID_FILE_ATTRIBUTES;
        }

        bool FileAttributes::isValid() const {
            return !isInvalid();
        }

        bool FileAttributes::isDirectory() const {
            return (dWord & FILE_ATTRIBUTE_DIRECTORY) != 0U;
        }

        bool FileAttributes::isFile() const {
            return !isDirectory();
        }

    } // namespace Windows
} // namespace MF
