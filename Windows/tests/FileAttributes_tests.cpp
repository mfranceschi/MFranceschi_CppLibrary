//
// Created by Utilisateur on 02/04/2022.
//

#include "MF/LightWindows.hpp"
#include "MF/Windows.hpp"
#include "tests_data.hpp"

TEST(FileAttributes, it_says_is_a_directory) {
    static const DWORD rawAttribute = 0x111;

    MF::Windows::FileAttributes fileAttributes(rawAttribute);

    EXPECT_TRUE(fileAttributes.isDirectory());
    EXPECT_FALSE(fileAttributes.isFile());
}

TEST(FileAttributes, it_says_is_a_file) {
    static const DWORD rawAttribute = 0x101;

    MF::Windows::FileAttributes fileAttributes(rawAttribute);

    EXPECT_TRUE(fileAttributes.isFile());
    EXPECT_FALSE(fileAttributes.isDirectory());
}

TEST(FileAttributes, it_says_is_valid) {
    static const DWORD rawAttribute = 0xFFFFFFFE;

    MF::Windows::FileAttributes fileAttributes(rawAttribute);
    EXPECT_TRUE(fileAttributes.isValid());
    EXPECT_FALSE(fileAttributes.isInvalid());
}

TEST(FileAttributes, it_says_is_invalid) {
    static const DWORD rawAttribute = 0xFFFFFFFF;

    MF::Windows::FileAttributes fileAttributes(rawAttribute);
    EXPECT_TRUE(fileAttributes.isInvalid());
    EXPECT_FALSE(fileAttributes.isValid());
}
