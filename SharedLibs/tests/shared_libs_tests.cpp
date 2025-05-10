//
// Created by Utilisateur on 10/04/2022.
//

#include "MF/Filesystem.hpp"
#include "MF/SharedLibs.hpp"
#include "shared_libs_tests_common.hpp"
#include "tests_data.hpp"

#if MF_WINDOWS
#    include <Windows.h>
#else
#    include <dlfcn.h>
#endif

using namespace MF::SharedLibs;

static void _cleanSearchPaths(const std::vector<std::string>& values) {
    for (const std::string& path : values) {
        try {
            RemoveFromSearchPaths(path);
        } catch (...) {
            // ignore
        }
    }
}

TEST(GetExtension, it_returns_a_value) {
    const auto* const value = GetExtension();

    ASSERT_THAT(value, ::testing::StartsWith("."));
}

TEST(AddToSearchPaths, it_throws_on_parameter_too_short) {
    const std::string wrongInput1;

    EXPECT_THROW(AddToSearchPaths(wrongInput1), std::invalid_argument);

    _cleanSearchPaths({wrongInput1});
}

TEST(AddToSearchPaths, it_can_add_and_remove_absolute_path) {
    const std::string correctInput1 = MF_SAMPLE_LIB_1_FOLDER;

    EXPECT_NO_THROW(AddToSearchPaths(correctInput1));
    EXPECT_NO_THROW(RemoveFromSearchPaths(correctInput1));
}

TEST(OpenSharedLib, it_cannot_open_without_correct_search_path) {
    std::shared_ptr<SharedLib> lib;
    EXPECT_THROW(lib = OpenExplicitly(MF_SAMPLE_LIB_1_NAME), SharedLib::element_not_found_exception)
        << "Even though we haven't specified a search path, the library has been opened. "
        << "Input provided: '" << MF_SAMPLE_LIB_1_NAME << "'.";

    if (HasFailure()) {
#if MF_WINDOWS
        static constexpr std::size_t MODULE_PATH_LENGTH = 1000 * 1000;
        std::vector<char> modulePath(MODULE_PATH_LENGTH);
        GetModuleFileNameA(
            static_cast<HMODULE>(lib->GetSystemItem()), modulePath.data(), MODULE_PATH_LENGTH);
        GTEST_FAIL() << "For reference, DLL path is: '" << modulePath.data() << "'.";
#endif
    }
}

TEST(OpenSharedLib, it_can_open_successfully_with_correct_search_path) {
    AddToSearchPaths(MF_SAMPLE_LIB_1_FOLDER);

    EXPECT_NO_THROW(OpenExplicitly(MF_SAMPLE_LIB_1_NAME));

    _cleanSearchPaths({MF_SAMPLE_LIB_1_FOLDER});
}

static constexpr auto dummyFunctionName = "dummy_request_1234";

class SampleLib1Tests : public OpenedSharedLibFixture {
   public:
    SampleLib1Tests() : OpenedSharedLibFixture(MF_SAMPLE_LIB_1_FOLDER, MF_SAMPLE_LIB_1_NAME) {
    }
};

TEST_F(SampleLib1Tests, it_can_return_the_system_item) {
    void* systemItem = sharedLib->GetSystemItem();

#if MF_WINDOWS
    HMODULE hmodule = static_cast<HMODULE>(systemItem);
    auto procAddress = GetProcAddress(hmodule, dummyFunctionName);
    EXPECT_EQ(procAddress, nullptr);
    EXPECT_EQ(GetLastError(), ERROR_PROC_NOT_FOUND);
#else
    // clear errors
    dlerror();

    void* result = dlsym(systemItem, dummyFunctionName);
    EXPECT_EQ(result, nullptr);
    EXPECT_NE(dlerror(), nullptr);
#endif
}

TEST_F(SampleLib1Tests, it_can_return_the_system_path) {
    std::string result = "";

    ASSERT_NO_THROW(result = sharedLib->GetSystemPath());

    // Reminder: the file separator used here is "/".
    EXPECT_EQ(result, MF_SAMPLE_LIB_1_FOLDER "/" MF_SAMPLE_LIB_1_EFFECTIVE_NAME);
}
