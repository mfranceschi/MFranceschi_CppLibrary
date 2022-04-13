//
// Created by Utilisateur on 10/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "my_lib_1.hpp"
#include "tests_data.hpp"

#if MF_UNIX
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
    EXPECT_THROW(OpenExplicitly(MF_SAMPLE_LIB_1_NAME), std::invalid_argument);
}

TEST(OpenSharedLib, it_can_open_successfully_with_correct_search_path) {
    AddToSearchPaths(MF_SAMPLE_LIB_1_FOLDER);

    EXPECT_NO_THROW(OpenExplicitly(MF_SAMPLE_LIB_1_NAME));

    _cleanSearchPaths({MF_SAMPLE_LIB_1_FOLDER});
}

class SampleLib1Tests : public ::testing::Test {
   protected:
    std::shared_ptr<SharedLib> sharedLib;

   public:
    void SetUp() override {
        AddToSearchPaths(MF_SAMPLE_LIB_1_FOLDER);

        sharedLib = OpenExplicitly(MF_SAMPLE_LIB_1_NAME);
    }

    void TearDown() override {
        RemoveFromSearchPaths(MF_SAMPLE_LIB_1_FOLDER);
    }
};

TEST_F(SampleLib1Tests, it_can_get_and_use_function) {
    auto return_true_retrieved = sharedLib->GetFunction<decltype(&return_true)>("return_true");

    int result = return_true_retrieved();

    ASSERT_EQ(result, 1);
}

TEST_F(SampleLib1Tests, it_can_return_the_system_item) {
    static constexpr auto dummyFunctionName = "dummy_request_1234";
    auto systemItem = sharedLib->GetSystemItem();

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
