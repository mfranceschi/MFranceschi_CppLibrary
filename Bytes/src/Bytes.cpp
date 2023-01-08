//
// Created by Utilisateur on 05/11/2022.
//

#include "MF/Bytes.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#if MF_WINDOWS
#    include <intrin.h>

#    include <algorithm>
#endif

namespace MF
{
    namespace Bytes
    {

        void Buffer::throwForInvalidIndex(std::size_t index) {
            if (index >= getSize()) {
                std::ostringstream oss;
                oss << "Index out of bounds: " << index << " is >= the buffer size, which is "
                    << getSize() << ".";
                throw std::out_of_range(oss.str());
            }
        }

        Buffer::iterator Buffer::begin() {
            return getWithCast<byte>();
        }

        Buffer::const_iterator Buffer::cbegin() const {
            return getWithCast<byte>();
        }

        Buffer::iterator Buffer::end() {
            return begin() + getSize();
        }

        Buffer::const_iterator Buffer::cend() const {
            return cbegin() + getSize();
        }

        class BufferWithRawPointer : public Buffer {
           public:
            BufferWithRawPointer(void* buffer, std::size_t size)
                : Buffer(), buffer(buffer), size(size) {
                if (buffer == nullptr) {
                    throw std::invalid_argument("The buffer pointer is equal to nullptr.");
                }
            }

            void* get() const override {
                return buffer;
            }

            std::size_t getSize() const override {
                return size;
            }

           private:
            void* const buffer;
            const std::size_t size;
        };

        std::shared_ptr<Buffer> makeBuffer(void* buffer, std::size_t size) {
            return std::make_shared<BufferWithRawPointer>(buffer, size);
        }

        class BufferWithUniquePointer : public Buffer {
           public:
            BufferWithUniquePointer(std::size_t size)
                : buffer(std::make_unique<byte[]>(size)), size(size) {
            }

            void* get() const override {
                return buffer.get();
            }

            std::size_t getSize() const override {
                return size;
            }

           private:
            const std::unique_ptr<byte[]> buffer;
            const std::size_t size;
        };

        std::shared_ptr<Buffer> makeBufferWithSize(std::size_t size) {
            return std::make_shared<BufferWithUniquePointer>(size);
        }

        std::uint16_t swapBytes(std::uint16_t value) {
#if MF_WINDOWS
            static_assert(
                sizeof(value) == sizeof(unsigned short), "Unexpected problem with type lengths.");
            return _byteswap_ushort(value);
#elif MF_GCC
            return __builtin_bswap16(value);
#else
#    error Not implemented!
#endif
        }

        std::uint32_t swapBytes(std::uint32_t value) {
#if MF_WINDOWS
            static_assert(
                sizeof(value) == sizeof(unsigned long), "Unexpected problem with type lengths.");
            return _byteswap_ulong(value);
#elif MF_GCC
            return __builtin_bswap32(value);
#else
#    error Not implemented!
#endif
        }

        std::uint64_t swapBytes(std::uint64_t value) {
#if MF_WINDOWS
            static_assert(
                sizeof(value) == sizeof(unsigned long long),
                "Unexpected problem with type lengths.");
            return _byteswap_uint64(value);
#elif MF_GCC
            return __builtin_bswap64(value);
#else
#    error Not implemented!
#endif
        }

        std::shared_ptr<Buffer> swapBytes(const std::shared_ptr<Buffer>& originalBuffer) {
            auto newBuffer = makeBufferWithSize(originalBuffer->getSize());

            switch (originalBuffer->getSize()) {
                case sizeof(std::uint8_t):
                    // Calling 'swapBytes' is a bit pointless in this case
                    newBuffer->getAt<std::uint8_t>(0) =
                        swapBytes(originalBuffer->getAt<std::uint8_t>(0));
                    break;
                case sizeof(std::uint16_t):
                    newBuffer->getAt<std::uint16_t>(0) =
                        swapBytes(originalBuffer->getAt<std::uint16_t>(0));
                    break;
                case sizeof(std::uint32_t):
                    newBuffer->getAt<std::uint32_t>(0) =
                        swapBytes(originalBuffer->getAt<std::uint32_t>(0));
                    break;
                case sizeof(std::uint64_t):
                    newBuffer->getAt<std::uint64_t>(0) =
                        swapBytes(originalBuffer->getAt<std::uint64_t>(0));
                    break;

                default:
                    std::reverse_copy(
                        originalBuffer->cbegin(), originalBuffer->cend(), newBuffer->begin());
                    break;
            }

            return newBuffer;
        }
    } // namespace Bytes
} // namespace MF
