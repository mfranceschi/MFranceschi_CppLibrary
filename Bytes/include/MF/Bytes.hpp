

#include <memory>
#include <string>
#include <vector>

#ifdef BIG_ENDIAN
#    undef BIG_ENDIAN
static constexpr long BIG_ENDIAN = 4321;
#endif

#ifdef LITTLE_ENDIAN
#    undef LITTLE_ENDIAN
static constexpr long LITTLE_ENDIAN = 4321;
#endif

namespace MF
{
    namespace Bytes
    {
        using byte = unsigned char;

        class Buffer {
            typedef byte* iterator;
            typedef const byte* const_iterator;

           public:
            virtual void* get() const = 0;

            /// Usage: @code myBuffer->getWithCast<byte>()
            template <typename Type>
            Type* getWithCast() const;

            template <typename Type>
            Type& getAt(std::size_t index);

            virtual std::size_t getSize() const = 0;

            iterator begin();
            const_iterator cbegin() const;
            iterator end();
            const_iterator cend() const;

           protected:
            Buffer() = default;

           private:
            void throwForInvalidIndex(std::size_t index);

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

        enum class Endianness { BIG_ENDIAN, LITTLE_ENDIAN };

        constexpr Endianness getCurrentEndianness();
        constexpr Endianness getNetworkEndianness();

        constexpr std::uint8_t swapBytes(std::uint8_t value);
        std::uint16_t swapBytes(std::uint16_t value);
        std::uint32_t swapBytes(std::uint32_t value);
        std::uint64_t swapBytes(std::uint64_t value);
        std::shared_ptr<Buffer> swapBytes(const std::shared_ptr<Buffer>& originalBuffer);

        template <Endianness Source_Endianness, Endianness Target_Endianness>
        struct DataConverter {
            static std::uint8_t read(std::uint8_t value);
            static std::uint16_t read(std::uint16_t value);
            static std::uint32_t read(std::uint32_t value);
            static std::uint64_t read(std::uint64_t value);
            static std::shared_ptr<Buffer> read(const std::shared_ptr<Buffer>& value);
        };

        template <>
        struct DataConverter<Endianness::LITTLE_ENDIAN, Endianness::LITTLE_ENDIAN> {
            static std::uint8_t read(std::uint8_t value) {
                return value;
            }

            static std::uint16_t read(std::uint16_t value) {
                return value;
            }

            static std::uint32_t read(std::uint32_t value) {
                return value;
            }

            static std::uint64_t read(std::uint64_t value) {
                return value;
            }

            static std::shared_ptr<Buffer> read(const std::shared_ptr<Buffer>& value) {
                return value;
            }
        };

        template <>
        struct DataConverter<Endianness::BIG_ENDIAN, Endianness::BIG_ENDIAN> {
            static std::uint8_t read(std::uint8_t value) {
                return value;
            }

            static std::uint16_t read(std::uint16_t value) {
                return value;
            }

            static std::uint32_t read(std::uint32_t value) {
                return value;
            }

            static std::uint64_t read(std::uint64_t value) {
                return value;
            }

            static std::shared_ptr<Buffer> read(const std::shared_ptr<Buffer>& value) {
                return value;
            }
        };

        template <>
        struct DataConverter<Endianness::LITTLE_ENDIAN, Endianness::BIG_ENDIAN> {
            static std::uint8_t read(std::uint8_t value) {
                return swapBytes(value);
            }

            static std::uint16_t read(std::uint16_t value) {
                return swapBytes(value);
            }

            static std::uint32_t read(std::uint32_t value) {
                return swapBytes(value);
            }

            static std::uint64_t read(std::uint64_t value) {
                return swapBytes(value);
            }

            static std::shared_ptr<Buffer> read(const std::shared_ptr<Buffer>& value) {
                return swapBytes(value);
            }
        };

        template <>
        struct DataConverter<Endianness::BIG_ENDIAN, Endianness::LITTLE_ENDIAN> {
            static std::uint8_t read(std::uint8_t value) {
                return swapBytes(value);
            }

            static std::uint16_t read(std::uint16_t value) {
                return swapBytes(value);
            }

            static std::uint32_t read(std::uint32_t value) {
                return swapBytes(value);
            }

            static std::uint64_t read(std::uint64_t value) {
                return swapBytes(value);
            }

            static std::shared_ptr<Buffer> read(const std::shared_ptr<Buffer>& value) {
                return swapBytes(value);
            }
        };

        template <typename Type>
        Type* Buffer::getWithCast() const {
            return reinterpret_cast<Type*>(get());
        }

        template <typename Type>
        Type& Buffer::getAt(std::size_t index) {
            throwForInvalidIndex(index);
            return getWithCast<Type>()[index];
        }

        constexpr Endianness getCurrentEndianness() {
            return MF_BIG_ENDIAN ? Endianness::BIG_ENDIAN : Endianness::LITTLE_ENDIAN;
        }

        constexpr Endianness getNetworkEndianness() {
            return Endianness::BIG_ENDIAN;
        }

        constexpr std::uint8_t swapBytes(std::uint8_t value) {
            return value;
        }
    } // namespace Bytes
} // namespace MF
