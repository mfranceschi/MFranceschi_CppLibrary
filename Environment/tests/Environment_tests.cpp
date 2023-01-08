//
// Created by MartinF on 25/06/2022.
//

#include "MF/Environment.hpp"
#include "tests_data.hpp"

using namespace MF::Environment;

// We might want to change those at build time for some OSes?
static const std::string VAR_THAT_EXISTS = "PATH";
static const std::string VAR_THAT_DOES_NOT_EXIST = "abcd1234";

class TestWithVarThatDoesNotExists : public ::testing::Test {
   protected:
    const std::string name = VAR_THAT_DOES_NOT_EXIST;

    void SetUp() override {
        assertDoesNotExist("From SetUp method");
    }

    void TearDown() override {
        unsetEnv(name);
    }

    void assertDoesNotExist(const std::string& messageToAppend = "") {
        ASSERT_FALSE(hasEnv(name)) << messageToAppend;
    }
};

TEST(HasEnv, it_returns_true_if_exists) {
    EXPECT_TRUE(hasEnv(VAR_THAT_EXISTS))
        << "WARNING: it will result in inaccurate checks for other tests.";
}

TEST(HasEnv, it_returns_false_if_does_not_exists) {
    ASSERT_FALSE(hasEnv(VAR_THAT_DOES_NOT_EXIST))
        << "WARNING: it will result in inaccurate checks for other tests.";
}

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
    const std::string defaultValue = "abc123-defaultValue";
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
    EXPECT_FALSE(hasEnv(name));
}

TEST_F(UnsetEnv2, it_is_no_op_if_not_exists) {
    EXPECT_NO_THROW(unsetEnv(name));
}

TEST(ListEnv, it_can_list_env_names) {
    auto result = listNames();
    ASSERT_FALSE(result.empty());

    for (const auto& str : result) {
        EXPECT_FALSE(str.empty());
        EXPECT_EQ(str.find('='), std::string::npos);
    }
}

TEST(ListEnv, it_can_list_all) {
    auto result = listAll();
    ASSERT_FALSE(result.empty());

    for (const auto& pair : result) {
        EXPECT_FALSE(pair.first.empty());

        EXPECT_EQ(pair.first.find('='), std::string::npos);
        EXPECT_EQ(pair.second.find('='), std::string::npos);
    }

    EXPECT_EQ(result.size(), listNames().size());
}

#if MF_WINDOWS
// Test UNICODE versions.
namespace UnicodeTests
{
    // We might want to change those at build time for some OSes?
    static const std::wstring VAR_THAT_EXISTS = L"PATH";
    static const std::wstring VAR_THAT_DOES_NOT_EXIST = L"abcd1234";

    class TestWithVarThatDoesNotExistsUnicode : public ::testing::Test {
       protected:
        const std::wstring name = VAR_THAT_DOES_NOT_EXIST;

        void SetUp() override {
            assertDoesNotExist(L"From SetUp method");
        }

        void TearDown() override {
            unsetEnv(name);
        }

        void assertDoesNotExist(const std::wstring& messageToAppend = L"") {
            ASSERT_FALSE(hasEnv(name)) << messageToAppend;
        }
    };

    TEST(HasEnvUnicode, it_returns_true_if_exists) {
        EXPECT_TRUE(hasEnv(VAR_THAT_EXISTS))
            << "WARNING: it will result in inaccurate checks for other tests.";
    }

    TEST(HasEnvUnicode, it_returns_false_if_does_not_exists) {
        ASSERT_FALSE(hasEnv(VAR_THAT_DOES_NOT_EXIST))
            << "WARNING: it will result in inaccurate checks for other tests.";
    }

    TEST(GetEnvUnicode, it_can_get_for_var_that_exists) {
        std::wstring result;
        EXPECT_NO_THROW(result = getEnv(VAR_THAT_EXISTS));
        EXPECT_FALSE(result.empty());
    }

    TEST(GetEnvOrDefaultUnicode, it_returns_existing_value_if_var_exists) {
        // It should have an existing, non-empty value.
        EXPECT_FALSE(getEnvOrDefault(VAR_THAT_EXISTS, L"").empty());
    }

    class GetEnvOrDefault2Unicode : public TestWithVarThatDoesNotExistsUnicode {};

    TEST_F(GetEnvOrDefault2Unicode, it_returns_default_value_if_not_exists) {
        // It should not exist --> return given default value.
        const std::wstring defaultValue = L"abc123-defaultValue";
        EXPECT_EQ(getEnvOrDefault(name, defaultValue), defaultValue);
    }

    class SetEnv2Unicode : public TestWithVarThatDoesNotExistsUnicode {};

    TEST_F(SetEnv2Unicode, it_can_set_for_unexisting) {
        const std::wstring valueToSet = L"123";
        setEnv(name, valueToSet);

        EXPECT_EQ(getEnv(name), valueToSet);
    }

    TEST_F(SetEnv2Unicode, it_can_set_for_existing) {
        setEnv(name, L"a");

        const std::wstring valueToSet = L"123";
        setEnv(name, valueToSet);

        EXPECT_EQ(getEnv(name), valueToSet);
    }

    TEST_F(SetEnv2Unicode, it_can_set_to_empty_value) {
        const std::wstring valueToSet;
        EXPECT_NO_THROW(setEnv(name, valueToSet));

        // We don't test how that affected the system because that is out of our control.
    }

    class UnsetEnv2Unicode : public TestWithVarThatDoesNotExistsUnicode {};

    TEST_F(UnsetEnv2Unicode, it_can_unset_env) {
        setEnv(name, L"a");

        unsetEnv(name);
        EXPECT_FALSE(hasEnv(name));
    }

    TEST_F(UnsetEnv2Unicode, it_is_no_op_if_not_exists) {
        EXPECT_NO_THROW(unsetEnv(name));
    }

    TEST(ListEnvUnicode, it_can_list_env_names) {
        auto result = listNames();
        ASSERT_FALSE(result.empty());

        for (const auto& str : result) {
            EXPECT_FALSE(str.empty());
            EXPECT_EQ(str.find('='), std::string::npos);
        }
    }

    TEST(ListEnvUnicode, it_can_list_all) {
        auto result = listAll();
        ASSERT_FALSE(result.empty());

        for (const auto& pair : result) {
            EXPECT_FALSE(pair.first.empty());

            EXPECT_EQ(pair.first.find('='), std::string::npos);
            EXPECT_EQ(pair.second.find('='), std::string::npos);
        }

        EXPECT_EQ(result.size(), listNames().size());
    }
} // namespace UnicodeTests
#endif
