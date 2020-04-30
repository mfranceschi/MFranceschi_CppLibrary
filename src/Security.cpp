//
// Created by mfran on 30/04/2020.
//

#include "Security.hpp"

static constexpr std::size_t MD5_DIGEST_LENGTH = 32;

const char* Hash::toCString() const {
    return internalSource;
}

std::string Hash::toString() const {
    return internalSource;
}

std::size_t Hash::getDigestLength() const {
    return internalDigestLength;
}

MD5Hash::MD5Hash (const std::string& source) :
    Hash(),
    internalDigestLength(MD5_DIGEST_LENGTH)
{
    internalSource = new char[source.length() + 1];
    // TODO copy source to internalSource
}

void MD5Hash::internalComputeHash() {
    using integer = std::int32_t;
    // TODO use https://fr.wikipedia.org/wiki/MD5#Pseudo-code
}
