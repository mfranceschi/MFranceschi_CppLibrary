

#include <memory>
#include <string>
#include <vector>

namespace MF
{
    namespace Bytes
    {
        using byte = unsigned char;

        class Buffer {
           public:
            virtual void* get() const = 0;

            /// Usage: @code myBuffer->getAs<byte>()
            template <typename Type>
            Type* getAs() const {
                return reinterpret_cast<Type*>(get());
            }

            virtual std::size_t getSize() const = 0;

           protected:
            Buffer() = default;

           public:
            // TODO: NoCopy, NoMove
            virtual ~Buffer() = default;
        };

        /**
         * Constructs a new buffer.
         * YOU are responsible of the memory.
         */
        std::shared_ptr<Buffer> makeBuffer(void* buffer, std::size_t size);

        /**
         * Constructs a new buffer which memory is self-managed.
         * It uses a unique_ptr under the hood.
         */
        std::shared_ptr<Buffer> makeBufferWithSize(std::size_t size);
    } // namespace Bytes
} // namespace MF
