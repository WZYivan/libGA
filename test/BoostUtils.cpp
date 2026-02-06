#include <iostream>
#include <string>
#include <vector>

#include <lga/BoostUtils>

int main()
{
    std::vector<double> dbls{1.0, 1.1, 1.2, 1.3, 1.4, 1.5};

    std::cout << lga::format2jsonArray<std::vector<double>>(dbls)
              << "\n";
    std::cout << lga::format2jsonArray(
                     dbls,
                     [](const auto &iter) -> std::string
                     {
                         return std::format("double->{:0.3f}", *iter);
                     })
              << "\n";

    std::vector<lga::Angle> agls{{1, 2, 3}, {4, 5, 6}};
    std::cout << lga::format2jsonArray(agls)
              << "\n";
    std::cout << lga::format2jsonArray(
                     agls,
                     [](const auto &iter) -> auto
                     {
                         return iter->format(lga::fmt::json_array);
                     })
              << "\n";
}