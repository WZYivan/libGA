#include <sstream>
#include <print>

#include <lga/Photogrammetry>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Catch::Matchers;
using namespace lga;

TEST_CASE("space resection")
{
    SECTION("ols")
    {
        Matrix img(4, 2), obj(4, 3), all(4, 5);
        all << -86.15, -68.99, 36589.41, 25273.32, 2195.17,
            -53.40, 82.21, 37631.08, 31324.51, 728.69,
            -14.78, -76.63, 39100.97, 24934.98, 2386.50,
            10.46, 64.43, 40426.54, 30319.81, 757.31;
        img = all.leftCols(2);
        obj = all.rightCols(3);
        img /= 1000;

        Interior interior{
            .x = 0,
            .y = 0,
            .f = 153.24 / 1000,
            .m = 50000};

        Space_Resection_Result result =
            spaceResection(interior, img, obj);

        REQUIRE(result.info == Iterative_Algo_Info::Success);

        Matrix img_inv = obj2img(obj, result.exterior, interior, result.rotate);

        for (int r = 0; r != 4; ++r)
        {
            for (int c = 0; c != 2; ++c)
            {
                REQUIRE_THAT(img(r, c) - img_inv(r, c), WithinAbs(0, 1e-3));
            }
        }
    }
}