#include <lga/OfficialWork>

int main()
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

    lga::config::angle.output_fmt = "{:1s} {:03d} d {:02d} m {:05.1f} s";

    std::cout << ">Frame:\n";
    result.frame.write<
        lga::Angle,
        double>("./frame.log.csv", hmdf::io_format::csv2);
    std::cout << ">Info frame:\n";
    result.info_frame.write<
        std::string,
        lga::Angle,
        double>("./info_frame.log.csv", hmdf::io_format::csv2);

    lga::reset(lga::config::angle);
}