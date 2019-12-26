//
// Created by mfran on 04/10/2019.
//

#ifndef MYWORKS_TEST0_GEOCOORD_HPP
#define MYWORKS_TEST0_GEOCOORD_HPP

#include <cmath>
#include <limits>
#include "Toolbox.hpp"

class GeoCoord {
public:
    // static
    static constexpr double LAT_MAX = +90.; // Inferior bound of latitudes range.
    static constexpr double LAT_MIN = -90.; // Superior bound of latitudes range.
    static constexpr double LON_MAX = +180.; // Inferior bound of longitudes range.
    static constexpr double LON_MIN = -180.; // Superior bound of longitudes range.

    static constexpr double RADIUS_OF_EARTH = 6371008.;

    static constexpr bool ValidateLongitude(double) noexcept;
    static constexpr bool ValidateLatitude(double) noexcept;
    static constexpr bool ValidateHeight(double) noexcept;

    // methods
    double DistanceFrom(const GeoCoord& other, double radius = RADIUS_OF_EARTH);

    // getters and setters
    constexpr double GetLongitude() const noexcept;
    constexpr double GetLatitude() const noexcept;
    constexpr double GetHeight() const noexcept;

    constexpr void SetLongitude(double);
    constexpr void SetLatitude(double);
    constexpr void SetHeight(double);

    // operators
    inline double operator- (const GeoCoord& other);

    // constructors
    constexpr GeoCoord(const GeoCoord&) = default;
    constexpr explicit GeoCoord(double longitude=0, double latitude=0, double height=0);
    explicit GeoCoord(const char* input, const char* sep1, const char* sep2=nullptr);
    ~GeoCoord() = default;

protected:
    double longitude, latitude, height;
};

constexpr GeoCoord::GeoCoord(double lon, double lat, double height) :
    longitude(Toolbox::Validate(lon, 0, ValidateLongitude(lon))),
    latitude(Toolbox::Validate(lat, 0, ValidateLatitude(lat))),
    height(Toolbox::Validate(height, 0, ValidateHeight(height)))
{}

double GeoCoord::operator-(const GeoCoord &other) {
    return DistanceFrom(other);
}

constexpr double GeoCoord::GetLongitude() const noexcept {return longitude;}
constexpr double GeoCoord::GetLatitude() const noexcept {return latitude;}
constexpr double GeoCoord::GetHeight() const noexcept {return height;}

constexpr void GeoCoord::SetLongitude(double lo) {longitude = ValidateLongitude(lo);}
constexpr void GeoCoord::SetLatitude(double la) {latitude = ValidateLatitude(la);}
constexpr void GeoCoord::SetHeight(double he) {height = ValidateHeight(he);}

constexpr bool GeoCoord::ValidateHeight(double h) noexcept {return h >= 0.;}
constexpr bool GeoCoord::ValidateLatitude(double la) noexcept {return la >= LAT_MIN && la <= LAT_MAX ;}
constexpr bool GeoCoord::ValidateLongitude(double lo) noexcept {return lo >= LON_MIN && lo <= LON_MAX;}

#endif //MYWORKS_TEST0_GEOCOORD_HPP
