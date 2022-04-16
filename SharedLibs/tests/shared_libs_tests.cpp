//
// Created by Utilisateur on 10/04/2022.
//

#include "MF/SharedLibs.hpp"
#include "my_lib_1_empty.hpp"
#include "tests_data.hpp"

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
