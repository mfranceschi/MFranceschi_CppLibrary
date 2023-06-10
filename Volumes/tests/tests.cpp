//
// Created by Utilisateur on 04/04/2023.
//

#include "MF/Volumes.hpp"
#include "tests_data.hpp"

MATCHER_P(OptionalContains, targetValue, "Optional contains value") {
    return arg->contains(targetValue);
}

MATCHER(OptionalIsEmpty, "Optional is empty") {
    return arg->isEmpty();
}

MATCHER(OptionalIsNotEmpty, "Optional contains value") {
    return !arg->isEmpty();
}

MATCHER(OptionalBoolIsNotEmpty, "OptionalBool contains value") {
    return !arg.isEmpty();
}

TEST(Volumes, it_can_list_all) {
    auto all = MF::Volumes::listAll();
    for (const auto& volume : all) {
        EXPECT_TRUE(volume);
    }
}

TEST(Volumes, it_has_basic_infos_for_all_volumes) {
    auto all = MF::Volumes::listAll();
    for (const auto& volume : all) {
        const bool mounted = volume->isMounted();

        const bool mustHaveZeroMountPoints = !mounted;
        EXPECT_EQ(volume->getMountPoints().empty(), mustHaveZeroMountPoints);
    }
}

TEST(Volumes, it_has_all_infos_if_mounted) {
    auto all = MF::Volumes::listAll();
    for (const auto& volume : all) {
        if (!volume->isMounted()) {
            continue;
        }
        EXPECT_THAT(volume->getTotalSize(), OptionalIsNotEmpty());
        EXPECT_THAT(volume->getBytesPerSector(), OptionalIsNotEmpty());
        EXPECT_THAT(volume->getFreeSize(), OptionalIsNotEmpty());
        EXPECT_THAT(volume->getUsedSize(), OptionalIsNotEmpty());

        EXPECT_EQ(
            volume->getTotalSize()->get(),
            volume->getUsedSize()->get() + volume->getFreeSize()->get())
            << "Total size should equal free+used size!";

        EXPECT_FALSE(volume->isRemovableDrive());
        EXPECT_FALSE(volume->isRamDisk());
        EXPECT_FALSE(volume->isRemoteDrive());
        EXPECT_FALSE(volume->isCdRomDrive());
        EXPECT_TRUE(volume->isNotRemovableDrive());

        EXPECT_NO_THROW(volume->getName());
        EXPECT_FALSE(volume->getFileSystemName().empty());
        EXPECT_THAT(volume->hasCaseSensitiveFileNamesSupport(), OptionalBoolIsNotEmpty());
        EXPECT_THAT(volume->hasUnicodeSupportForFileNames(), OptionalBoolIsNotEmpty());
        EXPECT_THAT(volume->hasFileBasedCompressionSupport(), OptionalBoolIsNotEmpty());
        // EXPECT_THAT(volume->isReadOnly(), OptionalBoolIsNotEmpty());
    }
}
