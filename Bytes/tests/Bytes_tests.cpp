//
// Created by Utilisateur on 05/11/2022.
//

#include "MF/Bytes.hpp"
#include "tests_data.hpp"

using namespace MF::Bytes;

static const std::size_t TESTS_BUFFER_SIZE = 123;

class BytesTestsWithMallocEdBuffer : public ::testing::Test {
   protected:
    void* rawBuffer = nullptr;

    void TearDown() override {
        if (rawBuffer != nullptr) {
            free(rawBuffer);
        }
    }
};

TEST_F(BytesTestsWithMallocEdBuffer, it_works_on_malloc_ed_buffer) {
    rawBuffer = malloc(TESTS_BUFFER_SIZE);

    auto buffer = makeBuffer(rawBuffer, TESTS_BUFFER_SIZE);
    EXPECT_EQ(buffer->get(), rawBuffer);
    EXPECT_EQ(buffer->getSize(), TESTS_BUFFER_SIZE);
}

TEST_F(BytesTestsWithMallocEdBuffer, it_works_on_malloc_ed_char_buffer) {
    rawBuffer = malloc(TESTS_BUFFER_SIZE);

    static_cast<char*>(rawBuffer)[0] = '0';
    static_cast<char*>(rawBuffer)[1] = '1';
    static_cast<char*>(rawBuffer)[2] = '2';

    auto buffer = makeBuffer(rawBuffer, TESTS_BUFFER_SIZE);
    EXPECT_EQ(buffer->get(), rawBuffer);
    EXPECT_EQ(buffer->getWithCast<char>()[0], '0');
    EXPECT_EQ(buffer->getWithCast<char>()[0], buffer->getAt<char>(0));
    EXPECT_EQ(buffer->getWithCast<char>()[1], '1');
    EXPECT_EQ(buffer->getWithCast<char>()[2], '2');
    EXPECT_EQ(buffer->getSize(), TESTS_BUFFER_SIZE);
}

TEST(BytesTestsWithSize, it_can_construct) {
    auto buffer = makeBufferWithSize(TESTS_BUFFER_SIZE);
    EXPECT_NE(buffer->get(), nullptr);
    EXPECT_EQ(buffer->getSize(), TESTS_BUFFER_SIZE);
}

TEST(BytesTestsWithSize, it_can_get_at_index) {
    auto buffer = makeBufferWithSize(TESTS_BUFFER_SIZE);

    for (std::size_t i = 0; i < TESTS_BUFFER_SIZE; i++) {
        EXPECT_NO_THROW(buffer->getAt<char>(i)) << "Index=" << i;
    }

    const std::vector<std::size_t> someWrongIndices = {
        TESTS_BUFFER_SIZE, TESTS_BUFFER_SIZE + 1, (std::size_t)-1};
    for (const std::size_t& wrongIndex : someWrongIndices) {
        EXPECT_THROW(buffer->getAt<char>(wrongIndex), std::exception) << "Index=" << wrongIndex;
    }
}
