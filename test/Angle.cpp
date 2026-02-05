// test/Angle.cpp

#include <print>
#include <stdexcept>
#include <regex>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <lga/Angle>

using namespace Catch::Matchers;

TEST_CASE("Angle constructor with double", "[angle][constructor]")
{
    SECTION("Zero angle")
    {
        lga::Angle angle(0.0);
        REQUIRE(angle.toRadian() == 0.0);
    }

    SECTION("Positive angle")
    {
        lga::Angle angle(1.5);
        REQUIRE_THAT(angle.toRadian(), WithinAbs(1.5, 1e-10));
    }

    SECTION("Negative angle")
    {
        lga::Angle angle(-1.5);
        REQUIRE_THAT(angle.toRadian(), WithinAbs(-1.5, 1e-10));
    }
}

TEST_CASE("Angle constructor with initializer list", "[angle][constructor]")
{
    SECTION("Valid DMS values")
    {
        auto angle = lga::Angle({45.0, 30.0, 30.0}); // 45°30'30"
        REQUIRE(angle.toRadian() > 0.0);             // Should be positive
    }

    SECTION("Single value")
    {
        auto angle = lga::Angle({lga::rad2deg(1.0)});
        REQUIRE_THAT(angle.toRadian(), WithinAbs(1.0, 1e-10));
    }

    SECTION("Empty list")
    {
        auto angle = lga::Angle({});
        REQUIRE_THAT(angle.toRadian(), WithinAbs(0.0, 1e-10));
    }

    SECTION("Negative value")
    {
        lga::Angle ang{-45, 30, 0};
        REQUIRE(ang.withinAbs(-lga::Angle(lga::deg2rad(45) + lga::min2rad(30))));
    }
}

TEST_CASE("Angle factory methods", "[angle][factory]")
{
    SECTION("fromSeconds")
    {
        auto angle = lga::Angle::fromSeconds(1800.0); // 30 minutes in seconds
        REQUIRE_THAT(angle.toRadian(), WithinAbs(lga::sec2rad(1800.0), 1e-10));
    }

    SECTION("fromMinutes")
    {
        auto angle = lga::Angle::fromMinutes(30.0);
        REQUIRE_THAT(angle.toRadian(), WithinAbs(lga::min2rad(30.0), 1e-10));
    }

    SECTION("fromDegrees")
    {
        auto angle = lga::Angle::fromDegrees(90.0);
        REQUIRE_THAT(angle.toRadian(), WithinAbs(lga::deg2rad(90.0), 1e-10));
    }

    SECTION("fromRadian")
    {
        auto angle = lga::Angle::fromRadian(1.57); // π/2
        REQUIRE_THAT(angle.toRadian(), WithinAbs(1.57, 1e-10));
    }

    SECTION("fromDMS")
    {
        auto angle = lga::Angle::fromDMS(45, 30, 30, false); // Positive
        REQUIRE(angle.toRadian() > 0.0);

        auto neg_angle = lga::Angle::fromDMS(45, 30, 30, true); // Negative
        REQUIRE(neg_angle.toRadian() < 0.0);
    }
}

TEST_CASE("Angle conversion methods", "[angle][conversion]")
{
    auto angle = lga::Angle::fromDegrees(90.0);

    SECTION("toSeconds")
    {
        REQUIRE_THAT(angle.toSeconds(), WithinAbs(90.0 * 3600.0, 0.1));
    }

    SECTION("toMinutes")
    {
        REQUIRE_THAT(angle.toMinutes(), WithinAbs(90.0 * 60.0, 0.1));
    }

    SECTION("toDegrees")
    {
        REQUIRE_THAT(angle.toDegrees(), WithinAbs(90.0, 0.001));
    }

    SECTION("toRadian")
    {
        REQUIRE_THAT(angle.toRadian(), WithinAbs(lga::deg2rad(90.0), 1e-10));
    }
}

TEST_CASE("Angle DMS extraction methods", "[angle][dms]")
{
    auto angle = lga::Angle::fromDMS(45, 30, 30, false);

    SECTION("degrees")
    {
        REQUIRE(angle.degrees() == 45);
    }

    SECTION("minutes")
    {
        REQUIRE(angle.minutes() == 30);
    }

    SECTION("seconds")
    {
        REQUIRE_THAT(angle.seconds(), WithinAbs(30.0, 0.001));
    }

    SECTION("toDMS tuple")
    {
        auto [d, m, s] = angle.toDMS();
        REQUIRE(d == 45);
        REQUIRE(m == 30);
        REQUIRE_THAT(s, WithinAbs(30.0, 0.001));
    }
}

TEST_CASE("Angle trigonometric functions", "[angle][trig]")
{
    auto angle = lga::Angle::fromDegrees(45.0);
    double sqrt2div2 = std::sqrt(2.0) / 2.0;
    SECTION("sin")
    {
        REQUIRE_THAT(angle.sin(), WithinAbs(sqrt2div2, 0.001));
    }

    SECTION("cos")
    {
        REQUIRE_THAT(angle.cos(), WithinAbs(sqrt2div2, 0.001));
    }

    SECTION("tan")
    {
        REQUIRE_THAT(angle.tan(), WithinAbs(1.0, 1e9));
    }

    SECTION("sec")
    {
        REQUIRE_THAT(angle.sec(), WithinAbs(1.0 / angle.cos(), 0.001));
    }

    SECTION("csc")
    {
        REQUIRE_THAT(angle.csc(), WithinAbs(1.0 / angle.sin(), 0.001));
    }

    SECTION("cot")
    {
        REQUIRE_THAT(angle.cot(), WithinAbs(1.0 / angle.tan(), 0.001));
    }
}

TEST_CASE("Utility conversion functions", "[angle][utils]")
{
    SECTION("sec2rad")
    {
        REQUIRE_THAT(lga::sec2rad(3600.0), WithinAbs(lga::deg2rad(1.0), 1e-10)); // 3600 sec = 1 deg
    }

    SECTION("min2rad")
    {
        REQUIRE_THAT(lga::min2rad(60.0), WithinAbs(lga::deg2rad(1.0), 1e-10)); // 60 min = 1 deg
    }

    SECTION("deg2rad")
    {
        REQUIRE_THAT(lga::deg2rad(180.0), WithinAbs(M_PI, 1e-10));
    }

    SECTION("rad2sec")
    {
        REQUIRE_THAT(lga::rad2sec(lga::deg2rad(1.0)), WithinAbs(3600.0, 0.1)); // 1 deg = 3600 sec
    }

    SECTION("rad2min")
    {
        REQUIRE_THAT(lga::rad2min(lga::deg2rad(1.0)), WithinAbs(60.0, 0.1)); // 1 deg = 60 min
    }

    SECTION("rad2deg")
    {
        REQUIRE_THAT(lga::rad2deg(M_PI), WithinAbs(180.0, 1e-10));
    }
}

TEST_CASE("dms2rad template function", "[angle][utils]")
{
    std::vector<double> dms = {45.0, 30.0, 30.0}; // 45°30'30"

    SECTION("valid input")
    {
        auto rad = lga::dms2rad(dms.begin(), dms.end());
        REQUIRE(rad > 0.0);
    }

    SECTION("invalid input size")
    {
        std::vector<double> invalid_dms = {45.0, 30.0, 1.0, 1.0}; // More than 4 elements
        REQUIRE_THROWS_AS(lga::dms2rad(invalid_dms.begin(), invalid_dms.end()), std::range_error);
    }
}

TEST_CASE("fromString method", "[angle][parsing]")
{
    SECTION("valid input")
    {
        std::string_view pattern = R"(([+-]?)(\d{1,3})\.(\d{2})(\d{2}))";
        REQUIRE_THROWS_AS(lga::Angle::fromString("invalid", pattern), std::invalid_argument);

        auto ang1 = lga::Angle::fromString("-1.23456").toString();
        auto ang2 = lga::Angle{-1, 23, 45.6}.toString();

        CAPTURE(ang1);
        CAPTURE(ang2);

        REQUIRE(lga::Angle::fromString("-1.23456")
                    .withinAbs(lga::Angle({-1, 23, 45.6})));
    }

    SECTION("invalid pattern")
    {
        REQUIRE_THROWS_AS(lga::Angle::fromString("+45.3030", "invalid[regex["), std::regex_error);
    }
}

TEST_CASE("toString method", "[angle][formatting]")
{
    auto angle = lga::Angle::fromDMS(45, 30, 30, false);

    SECTION("default format")
    {
        auto str = angle.toString();
        REQUIRE(!str.empty());
    }

    SECTION("custom format")
    {
        auto str = angle.toString("{:s}{:03d}.{:02d}{:05.2f}");
        REQUIRE(!str.empty());
    }
}

TEST_CASE("Angle edge cases", "[angle][edge]")
{
    SECTION("zero angle")
    {
        lga::Angle zero(0.0);
        REQUIRE(zero.degrees() == 0);
        REQUIRE(zero.minutes() == 0);
        REQUIRE_THAT(zero.seconds(), WithinAbs(0.0, 0.001));
    }

    SECTION("negative angles")
    {
        auto angle = lga::Angle::fromDMS(45, 30, 30, true); // negative
        REQUIRE(angle.toRadian() < 0.0);
    }

    SECTION("large angles")
    {
        auto angle = lga::Angle::fromDegrees(1000.0);
        REQUIRE(angle.toDegrees() > 999.0);
    }
}

TEST_CASE("Angle algebra operators", "[angle][algebra]")
{
    SECTION("add , sub and neg")
    {
        {
            lga::Angle
                ang1(lga::deg2rad(45)),
                ang2(lga::deg2rad(-45));
            REQUIRE((ang1 + ang2).withinAbs(lga::Angle(0.0)));
            REQUIRE((ang1 - (-ang2)).withinAbs(lga::Angle(0.0)));
        }
    }

    SECTION("multi and div")
    {
        lga::Angle ang({30});
        REQUIRE((ang * 2).withinAbs(lga::Angle({60})));
        REQUIRE((ang / 2).withinAbs(lga::Angle({15})));
        REQUIRE((ang / ang) == 1);
    }

    SECTION("mod")
    {
        lga::Angle
            ang1(lga::deg2rad(45)),
            ang2(lga::deg2rad(20));
        REQUIRE((ang1 % ang2).withinAbs(lga::Angle({5})));
    }
}

TEST_CASE("Normalization method")
{
    SECTION("norm")
    {
        lga::Angle ang{361, 0, 0};

        REQUIRE(ang.norm().withinAbs(lga::Angle{1, 0, 0}));
    }

    SECTION("normSym")
    {
        lga::Angle ang{-181, 0, 0};
        REQUIRE_THROWS_AS(ang.normSym(), lga::Not_Implement_Error);
    }
}

TEST_CASE("Compare operator", "[angle][operator]")
{
    SECTION("==")
    {
        lga::Angle ang1{1, 1, 1}, ang2{1, 1, 1.0000000000001};
        REQUIRE(ang1.eqApprox(ang2));
    }

    SECTION("><")
    {
        lga::Angle ang1{180, 0, 0}, ang2{-190, 0, 0}, ang3{360, 0, 0};
        REQUIRE(ang1.gt(ang2));
        REQUIRE(ang2.gtAbs(ang1));
        REQUIRE(ang1.ltAbs(ang2));
        REQUIRE(ang1.lt(ang3));
    }
}