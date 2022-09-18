//
// Created by MartinF on 25/06/2022.
//

#include "MF/Optionals.hpp"
#include "tests_data.hpp"

using namespace MF::Optionals;

TEST(Optionals, it_can_build_simple_instances) {
    OptionalPtr<int> emptyInt = empty<int>();
    EXPECT_TRUE(emptyInt->isEmpty());

    OptionalPtr<int> intWithOne = ofLvalue<int>(1);
    EXPECT_TRUE(intWithOne->isPresent());

    std::shared_ptr<int> pointerTo3 = std::make_shared<int>(3);
    OptionalPtr<int> with3 = ofSharedPointer<int>(pointerTo3);
    EXPECT_TRUE(with3->isPresent());
}

TEST(Optionals, it_says_is_empty_and_is_present) {
    OptionalPtr<int> emptyInt = empty<int>();
    EXPECT_TRUE(emptyInt->isEmpty());
    EXPECT_FALSE(emptyInt->isPresent());

    OptionalPtr<int> intWithOne = ofLvalue<int>(1);
    EXPECT_TRUE(intWithOne->isPresent());
    EXPECT_FALSE(intWithOne->isEmpty());
}

TEST(Optionals, it_gets_the_value) {
    OptionalPtr<int> emptyInt = empty<int>();
    OptionalPtr<int> intWithOne = ofLvalue<int>(1);

    EXPECT_THROW(emptyInt->get(), EmptyOptionalException);
    EXPECT_NO_THROW(intWithOne->get());
    EXPECT_EQ(intWithOne->get(), 1);

    EXPECT_EQ(emptyInt->getOrDefault(2), 2);
    EXPECT_EQ(intWithOne->getOrDefault(2), 1);
}

TEST(Optionals, it_can_get_or_run) {
    OptionalPtr<int> emptyInt = empty<int>();
    OptionalPtr<int> intWithOne = ofLvalue<int>(1);

    const auto intSupplier = []() -> const int& {
        static const int two = 2;
        return two;
    };

    EXPECT_EQ(emptyInt->getOrRun(intSupplier), 2);
    EXPECT_EQ(intWithOne->getOrRun(intSupplier), 1);

    const auto intSupplierWithCopy = []() {
        return 2;
    };

    EXPECT_EQ(emptyInt->getOrRunWithCopy(intSupplier), 2);
    EXPECT_EQ(intWithOne->getOrRunWithCopy(intSupplier), 1);
}

TEST(Optionals, it_says_contains) {
    OptionalPtr<int> emptyInt = empty<int>();
    OptionalPtr<int> intWithOne = ofLvalue<int>(1);

    EXPECT_FALSE(emptyInt->contains(0));
    EXPECT_FALSE(emptyInt->contains(1));

    EXPECT_FALSE(intWithOne->contains(0));
    EXPECT_TRUE(intWithOne->contains(1));
}

TEST(Optionals, it_can_filter) {
    OptionalPtr<int> intWith3 = ofLvalue(3);
    EXPECT_TRUE(intWith3
                    ->filter([](const int& value) {
                        return value == 1;
                    })
                    ->isEmpty());
    EXPECT_TRUE(intWith3
                    ->filter([](const int& value) {
                        printf("Result=%d for Value=%d\n", value == 3, value);
                        return value == 3;
                    })
                    ->contains(3));
    EXPECT_TRUE(empty<int>()
                    ->filter([](const int& value) {
                        return value == 1;
                    })
                    ->isEmpty());
}

TEST(Optionals, it_can_map) {
    OptionalPtr<int> intWith3 = ofLvalue(3);
    OptionalPtr<int> intWithSixAfterMap = intWith3->map<int>([](const int& value) {
        return value * 2;
    });

    EXPECT_TRUE(intWithSixAfterMap->contains(6));

    OptionalPtr<std::string> threeAsString = intWith3->map<std::string>([](const int& value) {
        return std::to_string(value);
    });
    EXPECT_TRUE(threeAsString->contains("3"));
}

TEST(Optionals, it_can_flatMap) {
    OptionalPtr<int> intWith3 = ofLvalue(3);
    OptionalPtr<int> intWithSixAfterFlatMap = intWith3->flatMap<int>([](const int& value) {
        return ofLvalue(value * 2);
    });
    OptionalPtr<int> emptyIntAfterFlatMap = intWith3->flatMap<int>([](const int&) {
        return empty<int>();
    });
    EXPECT_TRUE(intWithSixAfterFlatMap->contains(6));
    EXPECT_TRUE(emptyIntAfterFlatMap->isEmpty());

    OptionalPtr<std::string> threeAsString = intWith3->flatMap<std::string>([](const int& value) {
        return ofLvalue(std::to_string(value));
    });
    EXPECT_TRUE(threeAsString->contains("3"));
}

TEST(Optionals, it_can_use_this_or_run) {
    OptionalPtr<int> emptyInt = empty<int>();
    OptionalPtr<int> intWith3 = ofLvalue(3);
    const auto intProvider = []() {
        return ofLvalue(2);
    };

    EXPECT_TRUE(emptyInt->useThisOrRun(intProvider)->contains(2));
    EXPECT_TRUE(intWith3->useThisOrRun(intProvider)->contains(3));
}
