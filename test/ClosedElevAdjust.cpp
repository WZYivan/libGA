#include <vector>
#include <iostream>

#include <lga/OfficialWork>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Catch::Matchers;
using namespace lga;

TEST_CASE("closed elev")
{
    SECTION("assert")
    {
        std::vector<double>
            distance = {0.8, 0.5, 1.2, 0.5, 1.0},
            diff = {0.23, 0.26, -0.55, -0.45, 0.49};

        lga::Adjust_Frame_Result result = lga::closedElevAdjust(
            distance,
            diff,
            12.0);

        REQUIRE(frameAssert(CLOSED_ELEV, result));
    }
}
