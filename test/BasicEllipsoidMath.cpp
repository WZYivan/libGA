#include <print>
#include <iostream>

#include <lga/Geodesy>

int main()
{
    lga::Latitude B(lga::deg2rad(45));
    auto rc = lga::principleCurvatureRadius(B, lga::krassovsky);
    std::println("M = {}, N = {}, R = {}",
                 rc.m,
                 rc.n,
                 lga::meanCurvatureRadius(rc));

    double len = lga::meridianArcLength(B, lga::krassovsky);
    auto B_inv = lga::meridianArcBottom(len, lga::krassovsky);

    auto fmt = "{:s}{:d}d{:d}m{:f}s";
    std::println("B_from = {}, len = {}, B_to = {}, B_delta = {}",
                 B.toAngle().toString(fmt),
                 len,
                 B_inv.toAngle().toString(fmt),
                 (B.toAngle() - B_inv.toAngle()).toString(fmt));
}