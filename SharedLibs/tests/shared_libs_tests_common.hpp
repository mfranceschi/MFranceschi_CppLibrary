//
// Created by MartinF on 23/04/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_SHARED_LIBS_TESTS_COMMON_HPP
#define MFRANCESCHI_CPPLIBRARIES_SHARED_LIBS_TESTS_COMMON_HPP

#include "MF/SharedLibs.hpp"
#include "tests_data.hpp"

using namespace ::MF::SharedLibs;

class OpenedSharedLibFixture : public ::testing::Test {
   protected:
    std::shared_ptr<SharedLib> sharedLib;
    const std::string folder;
    const std::string name;

    explicit OpenedSharedLibFixture(const std::string& folder, const std::string& name)
        : folder(folder), name(name) {
    }
    virtual ~OpenedSharedLibFixture() = default;

   public:
    void SetUp() override {
        AddToSearchPaths(folder);

        sharedLib = OpenExplicitly(name);
    }

    void TearDown() override {
        RemoveFromSearchPaths(folder);
    }
};

struct MfSampleLib {
    std::string name;
    std::string folder;
    std::string effectiveName;
    bool shouldLoad;
};

static const std::vector<MfSampleLib> libs = {
    MfSampleLib{}, // Empty for easier indexing.
    MfSampleLib{MF_SAMPLE_LIB_1_NAME, MF_SAMPLE_LIB_1_FOLDER, MF_SAMPLE_LIB_1_EFFECTIVE_NAME, true},
    MfSampleLib{MF_SAMPLE_LIB_2_NAME, MF_SAMPLE_LIB_2_FOLDER, "", true},
    MfSampleLib{MF_SAMPLE_LIB_3_NAME, MF_SAMPLE_LIB_3_FOLDER, "", true},
    MfSampleLib{MF_SAMPLE_LIB_4_NAME, MF_SAMPLE_LIB_4_FOLDER, "", true},
    MfSampleLib{MF_SAMPLE_LIB_5_NAME, MF_SAMPLE_LIB_5_FOLDER, "", false},
};

#endif // MFRANCESCHI_CPPLIBRARIES_SHARED_LIBS_TESTS_COMMON_HPP
