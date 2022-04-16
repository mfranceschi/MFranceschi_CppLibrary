//
// Created by Utilisateur on 16/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "my_lib_2.hpp"
#include "tests_data.hpp"

#if MF_UNIX
#    include <dlfcn.h>
#endif

using namespace MF::SharedLibs;

class SampleLib2Tests : public ::testing::Test {
   protected:
    std::shared_ptr<SharedLib> sharedLib;

   public:
    void SetUp() override {
        AddToSearchPaths(MF_SAMPLE_LIB_2_FOLDER);

        sharedLib = OpenExplicitly(MF_SAMPLE_LIB_2_NAME);
    }

    void TearDown() override {
        RemoveFromSearchPaths(MF_SAMPLE_LIB_2_FOLDER);
    }
};

TEST_F(SampleLib2Tests, it_can_get_and_use_function) {
    auto return_true_retrieved = sharedLib->GetFunction<decltype(&return_true)>("return_true");

    int result = return_true_retrieved();

    ASSERT_EQ(result, 1);
}

TEST_F(SampleLib2Tests, it_can_return_the_system_item) {
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
