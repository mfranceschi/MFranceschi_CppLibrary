//
// Created by mfran on 26/12/2019.
//

#include "tests_datas.hpp"
#include "ArgumentsParser.hpp"

TEST(MultiItemsArgument, ValidNoMultiItems) {
    ArgumentsParser ap;
    const char** toto = new const char* [3];
    toto[0] = "item0";
    toto[1] = "item1";
    toto[2] = "item2";
    EXPECT_NO_THROW(ap.parse(3, toto));
}

TEST(MultiItemsArgument, ValidWithOneMultiItems) {
    ArgumentsParser ap;
    const char** toto = new const char* [3];
    toto[0] = "item0";
    toto[1] = "\"item1";
    toto[2] = "item2\"";
    EXPECT_NO_THROW(ap.parse(3, toto));
}

TEST(MultiItemsArgument, ValidWithOneSingleMultiItems) {
    ArgumentsParser ap;
    const char** toto = new const char* [3];
    toto[0] = "item0";
    toto[1] = "\"item1\"";
    toto[2] = "item2";
    EXPECT_NO_THROW(ap.parse(3, toto));
}

TEST(MultiItemsArgument, ValidWithThreeMultiItems) {
    ArgumentsParser ap;
    const char** toto = new const char* [8];
    toto[0] = "item0";
    toto[1] = "\"item1\"";
    toto[2] = "item2";
    toto[3] = "\"item3";
    toto[4] = "item 4";
    toto[5] = "item5\"";
    toto[6] = "\"item6";
    toto[7] = "item7\"";
    EXPECT_NO_THROW(ap.parse(8, toto));
}

TEST(MultiItemsArgument, InvalidWithOneUnfinished) {
    ArgumentsParser ap;
    const char** toto = new const char* [3];
    toto[0] = "item0";
    toto[1] = "\"item1";
    toto[2] = "item2";
    EXPECT_THROW(ap.parse(3, toto), UnfinishedMultiItemsArgument);
}

TEST(MultiItemsArgument, InvalidWithOneInvalid) {
    ArgumentsParser ap;
    const char** toto = new const char* [3];
    toto[0] = "item0";
    toto[1] = "item1\"";
    toto[2] = "item2";
    EXPECT_THROW(ap.parse(3, toto), InvalidMultiItemsArgument);
}

TEST(Quick, Quick0) {
    ArgumentsParser ap;
    const char** toto = new const char* [4];
    toto[0] = "item0";
    toto[1] = "1";
    toto[2] = "2";
    toto[3] = "";
    ap.parse(4, toto);
    for (const auto& truc : ap.get()) {
        EXPECT_EQ(truc.second.valueType, ValueType::INTEGER);
    }
}