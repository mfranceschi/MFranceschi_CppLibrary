//
// Created by Utilisateur on 25/03/2022.
//

#include "MF/Streams.hpp"
#include "tests_data.hpp"

bool isIntPair(int input) {
    return input % 2 == 0;
}
bool isIntNonZero(int input) {
    return input != 0;
}
double convertToDoubleAndInvert(int input) {
    return 1. / (double)input;
}

template <typename T>
std::ostream &operator<<(std::ostream &theOstream, const std::vector<T> &data) {
    theOstream << "Vector"
               << "[";
    for (const auto &value : data) {
        theOstream << value << ", ";
    }
    theOstream << "]";
    return theOstream;
}

TEST(UnpolishedTest, FromAnotherRepo) {
    using namespace MF::Containers;
    using std::cout;

    std::vector<int> vecint{2, 22, 7, 987, -2, 0};

    StreamPtr<int> a = Streams::fromCollection(vecint);
    ASSERT_EQ(a->collectToVector().size(), 6);

    StreamPtr<int> b = a->filter(isIntPair);
    auto bVector = b->collectToVector();
    ASSERT_LIST_CONTAINS(bVector, 2);
    ASSERT_LIST_CONTAINS(bVector, 22);
    ASSERT_LIST_CONTAINS(bVector, -2);
    ASSERT_LIST_CONTAINS(bVector, 0);
    ASSERT_EQ(bVector.size(), 4);

    StreamPtr<int> c = b->filter(isIntNonZero);
    auto cVector = c->collectToVector();
    ASSERT_LIST_CONTAINS(cVector, 2);
    ASSERT_LIST_CONTAINS(cVector, 22);
    ASSERT_LIST_CONTAINS(cVector, -2);
    ASSERT_EQ(cVector.size(), 3);

    StreamPtr<double> d = c->map<double>(convertToDoubleAndInvert);
    ASSERT_EQ(d->collectToVector().size(), 3);
}
