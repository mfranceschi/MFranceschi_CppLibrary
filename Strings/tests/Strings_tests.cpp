//
// Created by Utilisateur on 05/11/2022.
//

#include "MF/Strings.hpp"
#include "tests_data.hpp"

using namespace MF::Strings;

TEST(Strings_Contains, itWorks) {
    const std::string input = "abcde";
    EXPECT_TRUE(contains(input, std::string("a")));
    EXPECT_TRUE(contains(input, std::string("b")));
    EXPECT_TRUE(contains(input, std::string("abcd")));
    EXPECT_TRUE(contains(input, std::string("abcde")));
    EXPECT_TRUE(contains(input, std::string("de")));
    EXPECT_TRUE(contains(input, std::string("")));
    EXPECT_FALSE(contains(input, std::string("ac")));
    EXPECT_FALSE(contains(input, std::string(" ")));
    EXPECT_FALSE(contains(input, std::string("\\")));
}

TEST(Strings_Split, itWorks) {
    const std::string input = "boo:and:foo";
    const std::vector<std::string> expected1 = {"boo", "and", "foo"};
    const std::vector<std::string> expected2 = {"b", "", ":and:f"};
    const std::vector<std::string> expected3 = {"boo:and:foo"};

    EXPECT_THAT(split(input, std::string(":")), ::testing::ContainerEq(expected1));
    EXPECT_THAT(split(input, std::string("o")), ::testing::ContainerEq(expected2));
    EXPECT_THAT(split(input, std::string(" ")), ::testing::ContainerEq(expected3));
}

TEST(Strings_StartsWith, itWorks) {
    const std::string input = "abcde";
    EXPECT_TRUE(startsWith(input, std::string("a")));
    EXPECT_TRUE(startsWith(input, std::string("ab")));
    EXPECT_TRUE(startsWith(input, std::string("abcde")));
    EXPECT_FALSE(startsWith(input, std::string("b")));
    EXPECT_FALSE(startsWith(input, std::string("bcde")));
    EXPECT_FALSE(startsWith(input, std::string("ac")));
    EXPECT_FALSE(startsWith(input, std::string(" ")));

    EXPECT_TRUE(startsWith(input, 'a'));
    EXPECT_FALSE(startsWith(input, 'b'));
    EXPECT_FALSE(startsWith(input, 'e'));

    EXPECT_FALSE(startsWith(std::string(""), 'a'));
    EXPECT_FALSE(startsWith(std::string(""), std::string("a")));
    EXPECT_TRUE(startsWith(std::string(""), std::string("")));
}

TEST(Strings_EndsWith, itWorks) {
    const std::string input = "abcde";
    EXPECT_TRUE(endsWith(input, std::string("e")));
    EXPECT_TRUE(endsWith(input, std::string("de")));
    EXPECT_TRUE(endsWith(input, std::string("abcde")));
    EXPECT_FALSE(endsWith(input, std::string("d")));
    EXPECT_FALSE(endsWith(input, std::string("abcd")));
    EXPECT_FALSE(endsWith(input, std::string("ce")));
    EXPECT_FALSE(endsWith(input, std::string(" ")));
    EXPECT_FALSE(endsWith(input, std::string("\\")));

    EXPECT_TRUE(endsWith(input, 'e'));
    EXPECT_FALSE(endsWith(input, 'd'));
    EXPECT_FALSE(endsWith(input, 'a'));

    EXPECT_FALSE(endsWith(std::string(""), 'a'));
    EXPECT_FALSE(endsWith(std::string(""), std::string("a")));
    EXPECT_TRUE(endsWith(std::string(""), std::string("")));
}

TEST(Strings_Strip, itWorks) {
    EXPECT_EQ(strip(std::string("abc")), std::string("abc"));
    EXPECT_EQ(strip(std::string(" abc")), std::string("abc"));
    EXPECT_EQ(strip(std::string("abc ")), std::string("abc"));
    EXPECT_EQ(strip(std::string(" abc ")), std::string("abc"));
    EXPECT_EQ(strip(std::string(" \t  abc  \t \n \t ")), std::string("abc"));

    EXPECT_TRUE(strip(std::string("  ")).empty());
    EXPECT_TRUE(strip(std::string("")).empty());
}
