#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include "MF/LightWindows.hpp"

namespace MF
{
    namespace Windows
    {
        /**
         * Displays a message box with the given text, then terminates the program.
         */
        void ShowErrorMessage(const char* text);

        /**
         * Generates a newly-allocated (using the "new" operator) wchar_t array from the given
         * Multi-Bytes string.
         */
        std::wstring ConvertString(const char* utf8String);

        class FileAttributes {
           public:
            FileAttributes() = delete;
            FileAttributes(DWORD value);

            DWORD get() const;
            FileAttributes& set(DWORD value);

            bool isInvalid() const;
            bool isValid() const;

            bool isDirectory() const;
            bool isFile() const; /// defined as "not a directory"

           private:
            DWORD dWord;
        };

        static inline FileAttributes makeFileAttributes(DWORD value) {
            FileAttributes instance(value);
            if (instance.isInvalid()) {
                throw std::invalid_argument("Provided file attributes are invalid");
            }
            return instance;
        }

        namespace internals
        {
            template <typename ResourceType, typename Closer>
            class ResourceCloser {
               public:
                ResourceCloser(ResourceType resource, Closer closer)
                    : resource(resource), closer(closer) {
                }

                const ResourceType& get() const {
                    return resource;
                }

                ResourceType release() {
                    ResourceType theResource = resource;
                    resource = ResourceType();
                    closer = Closer();
                    return theResource;
                }

                virtual ~ResourceCloser() {
                    closer(resource);
                }

               private:
                Closer closer;
                ResourceType resource;
            };
        } // namespace internals

        class HandleCloser : public internals::ResourceCloser<HANDLE, decltype(&CloseHandle)> {
           public:
            HandleCloser(HANDLE handle)
                : ResourceCloser<HANDLE, decltype(&CloseHandle)>(handle, CloseHandle) {
            }

            bool isInvalid() const {
                return get() == INVALID_HANDLE_VALUE;
            }
        };

        class FileHandleCloser : public internals::ResourceCloser<HANDLE, decltype(&FindClose)> {
           public:
            FileHandleCloser(HANDLE handle)
                : ResourceCloser<HANDLE, decltype(&FindClose)>(handle, FindClose) {
            }

            bool isInvalid() const {
                return get() == INVALID_HANDLE_VALUE;
            }
        };
    } // namespace Windows
} // namespace MF
