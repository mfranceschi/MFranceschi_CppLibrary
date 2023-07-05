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
    return 1. / double(input);
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

TEST(Containers, stream_full) {
    using namespace MF::Containers;
    using std::cout;

    std::vector<int> vecint{2, 22, 7, 987, -2, 0};

    StreamPtr<int> streamA = Streams::fromCollection(vecint);
    ASSERT_EQ(streamA->collectToVector().size(), 6);

    StreamPtr<int> streamB = streamA->filter(isIntPair);
    auto bVector = streamB->collectToVector();
    ASSERT_LIST_CONTAINS(bVector, 2);
    ASSERT_LIST_CONTAINS(bVector, 22);
    ASSERT_LIST_CONTAINS(bVector, -2);
    ASSERT_LIST_CONTAINS(bVector, 0);
    ASSERT_EQ(bVector.size(), 4);

    const StreamPtr<int> streamC = streamB->filter(isIntNonZero);
    auto cVector = streamC->collectToVector();
    ASSERT_LIST_CONTAINS(cVector, 2);
    ASSERT_LIST_CONTAINS(cVector, 22);
    ASSERT_LIST_CONTAINS(cVector, -2);
    ASSERT_EQ(cVector.size(), 3);

    const StreamPtr<double> streamD = streamC->map<double>(convertToDoubleAndInvert);
    ASSERT_EQ(streamD->collectToVector().size(), 3);
}
