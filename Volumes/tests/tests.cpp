//
// Created by Utilisateur on 04/04/2023.
//

#include "MF/Volumes.hpp"
#include "tests_data.hpp"

TEST(aa, bb) {
    auto volumeInformation = MF::Volumes::get0();
    EXPECT_EQ(volumeInformation->getTotalSize(), 380377952256);
    EXPECT_EQ(volumeInformation->getBytesPerSector(), 512);
    EXPECT_EQ(volumeInformation->getFreeSize(), 61539627008);
    EXPECT_EQ(volumeInformation->getUsedSize(), 318838325248);

    EXPECT_FALSE(volumeInformation->isRemovableDrive());
    EXPECT_FALSE(volumeInformation->isRamDisk());
    EXPECT_FALSE(volumeInformation->isRemoteDrive());
    EXPECT_FALSE(volumeInformation->isCdRomDrive());
    EXPECT_TRUE(volumeInformation->isNotRemovableDrive());

    EXPECT_EQ(volumeInformation->getName(), std::string(""));
    EXPECT_EQ(volumeInformation->getFileSystemName(), std::string("NTFS"));
    EXPECT_TRUE(volumeInformation->hasCaseSensitiveFileNamesSupport());
    EXPECT_TRUE(volumeInformation->hasUnicodeSupportForFileNames());
    EXPECT_TRUE(volumeInformation->hasCompressionSupport());
    EXPECT_FALSE(volumeInformation->isReadOnly());

    EXPECT_EQ(volumeInformation->getBootSectorsCount(), 0);
}
