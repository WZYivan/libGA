#include <print>
#include <iostream>

#include <lga/Geodesy>

auto fmt = "{:0s}{:d}d{:d}m{:f}s";

int main()
{
    lga::Latitude B1(lga::dms2rad(47, 46, 52.647'0));
    lga::Longitude L1(lga::dms2rad(35, 49, 36.330'0));
    lga::Angle A1(lga::dms2rad(44, 12, 13.664));
    double S = 44'797.282'6;

    lga::Geodetic_Forward_Solve_Result rf =
        lga::bessel_formula_solve(
            B1,
            L1,
            S,
            A1,
            lga::krassovsky);
    std::println(
        "L2 = {}, B2 = {}, A21 = {}",
        rf.lon.toString(lga::fmt::dms),
        rf.lat.toString(lga::fmt::dms),
        rf.backward.toString(lga::fmt::dms));

    lga::Geodetic_Inverse_Solve_Result ri =
        lga::bessel_formula_solve(
            B1,
            L1,
            rf.lat,
            rf.lon,
            lga::krassovsky);
    std::println(
        "A12 = {}, A21 = {}, S = {}",
        ri.forward.toString(lga::fmt::dms),
        ri.backward.toString(lga::fmt::dms),
        ri.s);
}