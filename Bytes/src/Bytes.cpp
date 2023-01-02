//
// Created by Utilisateur on 05/11/2022.
//

#include "MF/Bytes.hpp"

#include <sstream>
#include <stdexcept>

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
    } // namespace Bytes
} // namespace MF
