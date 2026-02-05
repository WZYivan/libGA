#include <vector>
#include <iostream>

#include <lga/OfficialWork>

int main()
{
    std::vector<double>
        distance = {1.6, 2.1, 1.7, 2.0},
        diff = {2.331, 2.813, -2.224, 1.430};

    lga::DataFrame df = lga::attachedElevAdjust(
        distance,
        diff,
        45.286,
        49.579);

    df.write<
        std::ostream,
        std::string,
        double>(
        std::cout, hmdf::io_format::csv2);
}