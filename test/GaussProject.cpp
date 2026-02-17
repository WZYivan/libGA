#include <print>
#include <iostream>

#include <lga/Geodesy>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Catch::Matchers;

TEST_CASE("gauss project")
{
    SECTION("forward and inverse residual less than 1e-3")
    {
        lga::Longitude L(lga::deg2rad(115));
        for (int i = 1; i != 90; ++i)
        {
            lga::Latitude B(lga::deg2rad(i));
            lga::Geodetic_Coordinate gc(B, L);
            lga::Gauss_Project_Coordinate gpc = lga::gauss_project(
                gc,
                lga::param::geodesy.gauss_project_interval.six,
                lga::cgcs2000);
            lga::Geodetic_Coordinate gc_inv = lga::gauss_project(
                gpc,
                lga::cgcs2000);
            double
                rb = lga::rad2sec(gc.lat.rad() - gc_inv.lat.rad()),
                rl = lga::rad2sec(gc.lon.rad() - gc_inv.lon.rad());
            REQUIRE_THAT(rb, WithinAbs(0, 1e-3));
            REQUIRE_THAT(rl, WithinAbs(0, 1e-3));
        }
    }
}