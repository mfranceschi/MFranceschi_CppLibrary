//
// Created by MartinF on 25/06/2022.
//

#include "MF/Environment.hpp"
#include "tests_data.hpp"

using namespace MF::Environment;

// We might want to change those at build time for some OSes?
static const std::string VAR_THAT_EXISTS = "PATH";
static const std::string VAR_THAT_DOES_NOT_EXIST = "foobarbaz";

#define ASSERT_ENV_VAR_DOES_NOT_EXIST(name) ASSERT_TRUE(getEnvOrDefault(name, "").empty())
#define EXPECT_ENV_VAR_DOES_NOT_EXIST(name) EXPECT_TRUE(getEnvOrDefault(name, "").empty())

class TestWithVarThatExists : public ::testing::Test {
    // We might want to change it at build time for some OSes?
    static constexpr const char* VAR_THAT_WILL_EXIST = "PATH";

    const std::string name = VAR_THAT_WILL_EXIST;
};

class GetEnv2 : public TestWithVarThatExists {};

class TestWithVarThatDoesNotExists : public ::testing::Test {
   protected:
    const std::string name = VAR_THAT_DOES_NOT_EXIST;

    void SetUp() override {
        assertDoesNotExist("From SetUp method");
    }

    void assertDoesNotExist(const std::string& messageToAppend = "") {
        ASSERT_ENV_VAR_DOES_NOT_EXIST(name) << messageToAppend;
    }
};

TEST(GetEnv, it_can_get_for_var_that_exists) {
    std::string result;
    EXPECT_NO_THROW(result = getEnv(VAR_THAT_EXISTS));
    EXPECT_FALSE(result.empty());
}

TEST(GetEnvOrDefault, it_returns_existing_value_if_var_exists) {
    // It should have an existing, non-empty value.
    EXPECT_FALSE(getEnvOrDefault(VAR_THAT_EXISTS, "").empty());
}

class GetEnvOrDefault2 : public TestWithVarThatDoesNotExists {};

TEST_F(GetEnvOrDefault2, it_returns_default_value_if_not_exists) {
    // It should not exist --> return given default value.
    static const std::string defaultValue = "abc123-defaultValue";
    EXPECT_EQ(getEnvOrDefault(name, defaultValue), defaultValue);
}

class SetEnv2 : public TestWithVarThatDoesNotExists {};

TEST_F(SetEnv2, it_can_set_for_unexisting) {
    const std::string valueToSet = "123";
    setEnv(name, valueToSet);

    EXPECT_EQ(getEnv(name), valueToSet);
}

TEST_F(SetEnv2, it_can_set_for_existing) {
    setEnv(name, "a");

    const std::string valueToSet = "123";
    setEnv(name, valueToSet);

    EXPECT_EQ(getEnv(name), valueToSet);
}

TEST_F(SetEnv2, it_can_set_to_empty_value) {
    const std::string valueToSet;
    EXPECT_NO_THROW(setEnv(name, valueToSet));

    // We don't test how that affected the system because that is out of our control.
}

class UnsetEnv2 : public TestWithVarThatDoesNotExists {};

TEST_F(UnsetEnv2, it_can_unset_env) {
    setEnv(name, "a");

    unsetEnv(name);
    EXPECT_ENV_VAR_DOES_NOT_EXIST(name);
}

TEST_F(UnsetEnv2, it_is_no_op_if_not_exists) {
    EXPECT_NO_THROW(unsetEnv(name));
}
