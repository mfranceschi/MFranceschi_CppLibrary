//
// Created by Utilisateur on 16/04/2022.
//

#include "MF/Filesystem.hpp"
#include "MF/SharedLibs.hpp"
#include "my_lib_3_variables_and_init/lib.hpp"
#include "tests_data.hpp"

using namespace MF::SharedLibs;
using namespace MF::Filesystem;

static constexpr auto dummyFunctionName = "dummy_request_1234";

class SampleLib3Tests : public ::testing::Test {
   protected:
    std::shared_ptr<SharedLib> sharedLib;

    static const Filename_t loadFilename;
    static const Filename_t unloadFilename;

   public:
    void SetUp() override {
        AddToSearchPaths(MF_SAMPLE_LIB_3_FOLDER);

        sharedLib = OpenExplicitly(MF_SAMPLE_LIB_3_NAME);
    }

    void TearDown() override {
        RemoveFromSearchPaths(MF_SAMPLE_LIB_3_FOLDER);

        sharedLib.reset();

#if defined(_MSC_VER)
#    pragma push_macro("DeleteFile")
#    undef DeleteFile
#endif
        EXPECT_NO_THROW(deleteFile(loadFilename))
            << "Failed to remove the file created when LOADing the library.";
        EXPECT_NO_THROW(deleteFile(unloadFilename))
            << "Failed to remove the file created when UNLOADing the library.";
#if defined(_MSC_VER)
#    pragma pop_macro("DeleteFile")
#endif
    }
};

const Filename_t SampleLib3Tests::loadFilename = "./lib3_load.txt";
const Filename_t SampleLib3Tests::unloadFilename = "./lib3_unload.txt";

TEST_F(SampleLib3Tests, it_can_get_and_use_variable) {
    const int& thevariable_retrieved = sharedLib->GetVariable<int>("thevariable");

    EXPECT_NE(thevariable_retrieved, 1) << "Init function of shared lib was not run.";
    EXPECT_EQ(thevariable_retrieved, 2);
}

TEST_F(SampleLib3Tests, it_can_get_names_in_case_sensitive_manner) {
    void* variableWithLowerCaseV = sharedLib->GetPointer("thevariable");
    void* variableWithUpperCaseV = sharedLib->GetPointer("theVariable");

    EXPECT_NE(variableWithLowerCaseV, variableWithUpperCaseV);
}

TEST_F(SampleLib3Tests, it_creates_file_on_load) {
    EXPECT_TRUE(isFile(loadFilename));
}

TEST_F(SampleLib3Tests, it_creates_file_on_unload) {
    sharedLib.reset();
    EXPECT_TRUE(isFile(unloadFilename));
}
