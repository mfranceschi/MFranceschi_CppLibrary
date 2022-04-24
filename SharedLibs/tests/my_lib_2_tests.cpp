//
// Created by Utilisateur on 16/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "my_lib_2/lib.hpp"
#include "tests_data.hpp"

using namespace MF::SharedLibs;

static constexpr auto dummyFunctionName = "dummy_request_1234";

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

TEST_F(SampleLib2Tests, it_throws_if_function_not_found) {
    EXPECT_THROW(
        sharedLib->GetFunction<decltype(&return_true)>(dummyFunctionName),
        SharedLib::element_not_found_exception);
}
