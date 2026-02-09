#include <vector>
#include <iostream>
#include <print>

#include <lga/OfficialWork>

using namespace lga;

int main()
{
    std::vector<Angle> left_angle{
        {99, 1, 0},
        {167, 45, 36},
        {123, 11, 24},
        {189, 20, 36},
        {179, 59, 18},
        {129, 27, 24}};

    Angle
        angle_beg{237, 59, 30},
        angle_end{46, 45, 24};

    std::vector<double> distance{
        225.85, 139.03, 172.57, 100.07, 102.48};
    double
        x_beg = 2507.65,
        y_beg = 1215.64,
        x_end = 2166.70,
        y_end = 1757.28;

    config::angle.output_fmt = "{:s} {:03d} d {:02d} m {:02.3f} s";

    Adjust_Frame_Result result = connectingTraverseAdjust(
        left_angle,
        distance,
        angle_beg,
        angle_end,
        x_beg,
        y_beg,
        x_end,
        y_end);

    std::cout << "> Frame:\n";
    result.frame.write<
        std::ostream,
        double,
        Angle>(std::cout, hmdf::io_format::csv2);

    std::cout << "> Info frame:\n";
    result.info_frame.write<
        std::ostream,
        std::string,
        double,
        Angle>(std::cout, hmdf::io_format::csv2);
}