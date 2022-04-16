//
// Created by Utilisateur on 16/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "my_lib_3_variables_and_init.hpp"
#include "tests_data.hpp"

using namespace MF::SharedLibs;

static constexpr auto dummyFunctionName = "dummy_request_1234";

class SampleLib3Tests : public ::testing::Test {
   protected:
    std::shared_ptr<SharedLib> sharedLib;

   public:
    void SetUp() override {
        AddToSearchPaths(MF_SAMPLE_LIB_3_FOLDER);

        sharedLib = OpenExplicitly(MF_SAMPLE_LIB_3_NAME);
    }

    void TearDown() override {
        RemoveFromSearchPaths(MF_SAMPLE_LIB_3_FOLDER);
    }
};

TEST_F(SampleLib3Tests, it_can_get_and_use_variable) {
    int& return_true_retrieved = *(sharedLib->GetFunction<int*>("the_variable"));

    EXPECT_NE(return_true_retrieved, 1);
}
// TODO DO OTHER TESTS WITH LOAD ETC
