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

#endif // MFRANCESCHI_CPPLIBRARIES_SHARED_LIBS_TESTS_COMMON_HPP
