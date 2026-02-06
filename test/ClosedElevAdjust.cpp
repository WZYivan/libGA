#include <vector>
#include <iostream>

#include <lga/OfficialWork>

int main()
{
    std::vector<double>
        distance = {0.8, 0.5, 1.2, 0.5, 1.0},
        diff = {0.23, 0.26, -0.55, -0.45, 0.49};

    lga::Adjust_Result result = lga::closedElevAdjust(
        distance,
        diff,
        12.0);

    std::cout << "> Frame:\n";
    result.frame.write<
        std::ostream,
        std::string,
        double>(
        std::cout, hmdf::io_format::csv2);

    std::cout << "> Info frame:\n";
    result.info_frame.write<
        std::ostream,
        std::string,
        double>(
        std::cout, hmdf::io_format::csv2);
}