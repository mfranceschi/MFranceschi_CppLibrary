//
// Created by MartinF on 25/06/2022.
//

#include "MF/Optionals.hpp"
#include "tests_data.hpp"

using namespace MF::Optionals;

TEST(Optionals, it_can_build_simple_instances) {
    Optional<int> emptyInt = empty<int>();
    EXPECT_TRUE(emptyInt.isEmpty());

    Optional<int> intWithOne = of<int>(1);
    EXPECT_TRUE(intWithOne.isPresent());

    std::shared_ptr<int> pointerTo3 = std::make_shared<int>(3);
    Optional<int> with3 = of<int>(pointerTo3);
    EXPECT_TRUE(with3.isPresent());
}

TEST(Optionals, it_says_is_empty_and_is_present) {
    Optional<int> emptyInt = empty<int>();
    EXPECT_TRUE(emptyInt.isEmpty());
    EXPECT_FALSE(emptyInt.isPresent());

    Optional<int> intWithOne = of<int>(1);
    EXPECT_TRUE(intWithOne.isPresent());
    EXPECT_FALSE(intWithOne.isEmpty());
}

TEST(Optionals, it_gets_the_value) {
    Optional<int> emptyInt = empty<int>();
    Optional<int> intWithOne = of<int>(1);

    EXPECT_THROW(emptyInt.get(), EmptyOptionalException);
    EXPECT_NO_THROW(intWithOne.get());
    EXPECT_EQ(intWithOne.get(), 1);

    EXPECT_EQ(emptyInt.getOrDefault(2), 2);
    EXPECT_EQ(intWithOne.getOrDefault(2), 1);
}

TEST(Optionals, it_says_contains) {
    Optional<int> emptyInt = empty<int>();
    Optional<int> intWithOne = of<int>(1);

    EXPECT_FALSE(emptyInt.contains(0));
    EXPECT_FALSE(emptyInt.contains(1));

    EXPECT_FALSE(intWithOne.contains(0));
    EXPECT_TRUE(intWithOne.contains(1));
}

TEST(Optionals, it_does_something_if_present) {
    Optional<int> intWith3 = of(3);
    int fetchedValue = -1;
    const auto theLambda = [&fetchedValue](const int& valueFromOptional) {
        fetchedValue = valueFromOptional;
    };

    intWith3.ifPresent(theLambda);
    EXPECT_EQ(fetchedValue, 3);

    Optional<int> theEmpty = empty<int>();
    bool isPresent = false;
    const auto theLambda2 = [&isPresent](const int&) {
        isPresent = true;
    };
    EXPECT_FALSE(isPresent);
}

TEST(Optionals, it_can_filter) {
    Optional<int> intWith3 = of(3);
    EXPECT_TRUE(intWith3
                    .filter([](const int& value) {
                        return value == 1;
                    })
                    .isEmpty());
    EXPECT_TRUE(intWith3
                    .filter([](const int& value) {
                        return value == 3;
                    })
                    .contains(3));
    EXPECT_TRUE(empty<int>()
                    .filter([](const int& value) {
                        return value == 1;
                    })
                    .isEmpty());
}

TEST(Optionals, it_can_map) {
    Optional<int> intWith3 = of(3);
    Optional<int> intWithSixAfterMap = intWith3.map<int>([](const int& value) {
        return value * 2;
    });

    EXPECT_TRUE(intWithSixAfterMap.contains(6));

    Optional<std::string> threeAsString = intWith3.map<std::string>([](const int& value) {
        return std::to_string(value);
    });
    EXPECT_TRUE(threeAsString.contains("3"));
}

TEST(Optionals, it_can_flatMap) {
    Optional<int> intWith3 = of(3);
    Optional<int> intWithSixAfterFlatMap = intWith3.flatMap<int>([](const int& value) {
        return of(value * 2);
    });
    Optional<int> emptyIntAfterFlatMap = intWith3.flatMap<int>([](const int& value) {
        return empty<int>();
    });
    EXPECT_TRUE(intWithSixAfterFlatMap.contains(6));
    EXPECT_TRUE(emptyIntAfterFlatMap.isEmpty());

    Optional<std::string> threeAsString = intWith3.flatMap<std::string>([](const int& value) {
        return of(std::to_string(value));
    });
    EXPECT_TRUE(threeAsString.contains("3"));
}
