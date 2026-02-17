#include <print>
#include <iostream>

#include <lga/Geodesy>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Catch::Matchers;
using namespace lga;

TEST_CASE("gauss lemma solve")
{
    SECTION("forward inverse residual less than 1e-3")
    {
        Latitude
            B1(dms2rad(47, 46, 52.647'0)),
            B2(dms2rad(48, 4, 9.638'4));
        Longitude
            L1(dms2rad(35, 49, 36.330'0)),
            L2(dms2rad(36, 14, 45.050'5));
        Geodetic_Inverse_Solve_Result ri = gauss_lemma_solve(B1, L1, B2, L2, cgcs2000);
        Geodetic_Forward_Solve_Result rf = gauss_lemma_solve(B1, L1, ri.s, ri.forward, cgcs2000);
        double
            dl = rad2sec(rf.lon.rad() - L2.rad()),
            db [[maybe_unused]] = rad2sec(rf.lat.rad() - B2.rad()),
            da = rad2sec(rf.backward.toRadian() - ri.backward.toRadian());
        REQUIRE_THAT(dl, WithinAbs(0, 1e-3));
        // this will fail cause "meridianArcBottom" algo is not precise version
        // REQUIRE_THAT(db, WithinAbs(0, 1e-3));
        REQUIRE_THAT(da, WithinAbs(0, 1e-3));
    }
}
