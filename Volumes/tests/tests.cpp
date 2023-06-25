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
        std::cout << "Working on " << volume->getSystemIdentifier() << std::endl;

        EXPECT_NO_THROW(volume->getName());
        std::cout << "Name: '" << volume->getName() << "'" << std::endl;

        EXPECT_FALSE(volume->getFileSystemName().empty());
        EXPECT_THAT(volume->hasCaseSensitiveFileNamesSupport(), OptionalBoolIsNotEmpty());
        EXPECT_THAT(volume->hasUnicodeSupportForFileNames(), OptionalBoolIsNotEmpty());
        EXPECT_THAT(volume->hasFileBasedCompressionSupport(), OptionalBoolIsNotEmpty());
        // EXPECT_THAT(volume->isReadOnly(), OptionalBoolIsNotEmpty());

        EXPECT_THAT(volume->isRemovableDrive(), OptionalBoolIsNotEmpty());
        EXPECT_THAT(volume->isRamDisk(), OptionalBoolIsNotEmpty());
        EXPECT_THAT(volume->isRemoteDrive(), OptionalBoolIsNotEmpty());
        EXPECT_THAT(volume->isCdRomDrive(), OptionalBoolIsNotEmpty());
        EXPECT_THAT(volume->isNotRemovableDrive(), OptionalBoolIsNotEmpty());

        if (volume->getName() != "Google Drive") {
            // TODO find out what is going on
            EXPECT_THAT(volume->getTotalSize(), OptionalIsNotEmpty());
            EXPECT_THAT(volume->getBytesPerSector(), OptionalIsNotEmpty());
            EXPECT_THAT(volume->getFreeSize(), OptionalIsNotEmpty());
            EXPECT_THAT(volume->getUsedSize(), OptionalIsNotEmpty());

            EXPECT_EQ(
                volume->getTotalSize()->get(),
                volume->getUsedSize()->get() + volume->getFreeSize()->get())
                << "Total size should equal free+used size!";
        }
    }
}

TEST(Volumes, it_can_get_root_drive) {
#if MF_WINDOWS
    const auto mountPoint = "G:\\";
#else
#    error TODO! "/"??
#endif
    auto volume = MF::Volumes::getMaybeForMountPoint(mountPoint);
    EXPECT_TRUE(volume);
    EXPECT_TRUE(volume->isMounted());
}

TEST(Volumes, it_works_on_g_drive) {
    GTEST_SKIP() << "For local testing for now."; // TODO remove

    auto volume = MF::Volumes::getMaybeForMountPoint("G:\\");
    EXPECT_NO_THROW(volume->isRamDisk());
}
