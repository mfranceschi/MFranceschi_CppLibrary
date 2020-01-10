//
// Created by mfran on 04/10/2019.
//

#include <cmath>
#include <cassert>
#include <cstring>
#include <cstdlib>

#include "GeoCoord.hpp"

double GeoCoord::DistanceFrom(const GeoCoord &other, double radius) {
    double distance_angular = std::acos(
            std::sin(latitude) * std::sin(other.latitude) +
            std::cos(latitude)*std::cos(other.latitude)*std::cos(other.longitude-longitude));
    double distance_in_meters = distance_angular * radius;
    return distance_in_meters;
}

GeoCoord::GeoCoord(const char *input, const char *sep1, const char *sep2) :
    GeoCoord()
{
    // Find 1st occurrence of sep1
    // If sep2 then find 1st occurrence of sep2 after sep1
    assert(input); assert(sep1);
    const char* sep1_addr = std::strstr(input, sep1);
    assert(sep1_addr);
    const char* sep2_addr = ((sep2 == nullptr || *sep2 == '\0') ? nullptr : std::strstr(sep1+1, sep2));
    char* return_ptr;

    // Extract latitude between 0 and sep1
    latitude = std::strtod(input, &return_ptr);
    assert(return_ptr == sep1_addr);
    input = sep1_addr + std::strlen(sep1);

    // Extract longitude between sep1 and (sep2 or end).
    longitude = std::strtod(input, &return_ptr);
    if (sep2_addr) {
        assert(return_ptr == sep2_addr);
        input = sep2_addr + std::strlen(sep2);
        // If sep2 then extract height between sep2 and end.
        height = std::strtod(input, nullptr);
    }
}
