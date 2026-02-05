#include <print>
#include <iostream>

#include <lga/Geodesy>

int main()
{
    lga::Latitude B(lga::deg2rad(45));
    auto rc = lga::principleCurvatureRadius(B, lga::krasovski);
    std::println("M = {}, N = {}, R = {}",
                 rc.m,
                 rc.n,
                 lga::meanCurvatureRadius(rc));

    double len = lga::meridianArcLength(B, lga::krasovski);
    auto B_inv = lga::meridianArcBottom(len, lga::krasovski);

    auto fmt = "{:s}{:d}d{:d}m{:f}s";
    std::println("B_from = {}, len = {}, B_to = {}, B_delta = {}",
                 B.toAngle().toString(fmt),
                 len,
                 B_inv.toAngle().toString(fmt),
                 (B.toAngle() - B_inv.toAngle()).toString(fmt));
}