#include <filesystem>

#include <lga/OfficialWork>
#include <lga/BoostUtils>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace Catch::Matchers;
using namespace lga;
namespace fs = std::filesystem;
namespace bst_json = boost::json;

TEST_CASE("closed traverse")
{
    SECTION("assert")
    {
        std::vector<lga::Angle> left_angle = {
            {107, 48, 30},
            {73, 0, 24},
            {89, 33, 48},
            {89, 36, 30}};
        std::vector<double> distance = {
            105.22, 80.18, 129.34, 78.16};
        lga::Angle angle_beg{125, 30, 0};
        double x_beg = 500, y_beg = 600;
        lga::Adjust_Frame_Result result =
            lga::closedTraverseAdjust(
                left_angle,
                distance,
                angle_beg,
                x_beg,
                y_beg);

        REQUIRE(frameAssert(CLOSED_TRA, result));
    }

    SECTION("json")
    {
        fs::path data_path{"test/data/closed_traverse_test_data.json"};
        std::ifstream ifs(data_path, std::ios::in);
        std::stringstream buffer;
        buffer << ifs.rdbuf();

        auto json = bst_json::parse(buffer.str()).as_object();
        auto out = bst_json::object{};

        lga::config::angle.output_fmt = lga::fmt::json_array;
        bst_json::parse_options j_opt{};

        for (const auto &[k, v] : json)
        {
            const bst_json::object &sub_json = v.as_object();
            const bst_json::array
                &j_distance = sub_json.at("d").as_array(),
                &j_angle = sub_json.at("a").as_array(),
                &j_angle_beg = sub_json.at("ab").as_array();
            const bst_json::value
                &j_x_beg = sub_json.at("x"),
                &j_y_beg = sub_json.at("y");

            std::vector<double> distance{
                j_distance |
                std::views::transform(
                    [](const bst_json::value &val) -> double
                    {
                        return lga::enforce2double(val);
                    }) |
                std::ranges::to<std::vector<double>>()};

            std::vector<lga::Angle> angle{
                j_angle |
                std::views::transform(
                    [](const bst_json::value &val) -> lga::Angle
                    {
                        return lga::enforce2angle(val.as_array());
                    }) |
                std::ranges::to<std::vector<lga::Angle>>()};

            lga::Angle angle_beg{lga::enforce2angle(j_angle_beg)};
            double
                x_beg = lga::enforce2double(j_x_beg),
                y_beg = lga::enforce2double(j_y_beg);

            auto result =
                lga::closedTraverseAdjust(
                    angle,
                    distance,
                    angle_beg,
                    x_beg,
                    y_beg);

            std::ostringstream oss{};
            result.frame.write<
                std::ostream,
                double,
                lga::Angle>(oss, hmdf::io_format::json);

            std::string str = oss.str();

            lga::preprocessJsonStr(str, j_opt);
            out[k] = bst_json::parse(str, {}, j_opt);
        }

        std::cout << out;
    }
}
