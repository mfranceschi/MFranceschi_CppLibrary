//
// Created by mfran on 30/04/2020.
//

#include <bitset>
#include <vector>
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

Hash::~Hash() {
    delete [] internalSource;
    delete [] internalDigest;
}

Hash::Hash(std::size_t digestLength) :
    internalDigestLength(digestLength) {}

void Hash::internalCallComputeHash() {
    return internalComputeHash();
}

MD5Hash::MD5Hash (const std::string& source) :
    Hash(MD5_DIGEST_LENGTH)
{
    internalSource = new char[source.length() + 1];
    // TODO copy source to internalSource

    internalCallComputeHash();
}

void MD5Hash::internalComputeHash() {
    using integer = std::uint32_t;
    using std::bitset;

    static constexpr unsigned int CHUNK_SIZE = 512;
    bitset<CHUNK_SIZE> chunk;
    const unsigned int nbChunks = std::strlen(internalSource) / CHUNK_SIZE + 1;

    static const integer s[64] = {
            7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
            5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
            4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
            6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };
    static const integer k[64] = {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
            0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
            0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
            0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
            0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
            0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
            0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
            0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
            0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
            0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
            0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
            0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
            0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
            0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
            0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
            0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };

    // Initialize variables:
    integer a0 = 0x67452301;   // A
    integer b0 = 0xefcdab89;   // B
    integer c0 = 0x98badcfe;   // C
    integer d0 = 0x10325476;   // D

    for (unsigned int i = 0; i < nbChunks; i++) {
        bitset<32> words[16];


    }

    // TODO use https://fr.wikipedia.org/wiki/MD5#Pseudo-code
    // https://en.cppreference.com/w/cpp/utility/bitset/bitset
    // https://en.cppreference.com/w/cpp/container/vector_bool
}
