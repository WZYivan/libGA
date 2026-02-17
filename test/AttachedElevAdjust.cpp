#include <vector>
#include <iostream>

#include <lga/OfficialWork>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Catch::Matchers;
using namespace lga;

TEST_CASE("attached elev adjust")
{
    SECTION("all")
    {
        std::vector<double>
            distance = {1.6, 2.1, 1.7, 2.0},
            diff = {2.331, 2.813, -2.224, 1.430};

        Adjust_Frame_Result result = attachedElevAdjust(distance, diff, 45.286, 49.579);

        REQUIRE(frameAssert(ATTACHED_ELEV, result));
    }
}
