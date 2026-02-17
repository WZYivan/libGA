#include <print>
#include <iostream>

#include <lga/Geodesy>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Catch::Matchers;
using namespace lga;

TEST_CASE("basic ellipsoid math")
{
    SECTION("meridian arc")
    {
        for (int i = 1; i != 6; ++i)
        {
            Latitude B(deg2rad(15 * i));
            double len = meridianArcLength(B, krassovsky);
            Latitude B_inv = meridianArcBottom(len, krassovsky);
            double db = rad2sec(B.rad() - B_inv.rad());
            REQUIRE_THAT(db, WithinAbs(0, 1e-3));
        }
    }
}