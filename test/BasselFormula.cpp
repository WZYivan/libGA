#include <print>
#include <iostream>

#include <lga/Geodesy>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Catch::Matchers;
using namespace lga;

TEST_CASE("bessel formula solve")
{
    SECTION("forward inverse residual less than 1e-3")
    {
        Latitude B1(lga::dms2rad(47, 46, 52.647'0));
        Longitude L1(lga::dms2rad(35, 49, 36.330'0));
        Angle A1(lga::dms2rad(44, 12, 13.664));
        double S = 44'797.282'6;

        Geodetic_Forward_Solve_Result rf = bessel_formula_solve(B1, L1, S, A1, krassovsky);
        Geodetic_Inverse_Solve_Result ri = bessel_formula_solve(B1, L1, rf.lat, rf.lon, krassovsky);
        double
            da = rad2sec(A1.toRadian() - ri.forward.toRadian()),
            ds = S - ri.s;
        REQUIRE_THAT(da, WithinAbs(0, 1));
        REQUIRE_THAT(ds, WithinAbs(0, 1));
    }
}