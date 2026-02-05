#include <print>
#include <iostream>

#include <lga/Geodesy>

using namespace lga;

int main()
{

    Latitude
        B1(dms2rad(47, 46, 52.647'0)),
        B2(dms2rad(48, 4, 9.638'4));
    Longitude
        L1(dms2rad(35, 49, 36.330'0)),
        L2(dms2rad(36, 14, 45.050'5));

    std::println("L1 = {}, B1 = {}, L2 = {}, B2 = {}",
                 L1.toString(fmt::dms),
                 B1.toString(fmt::dms),
                 L2.toString(fmt::dms),
                 B2.toString(fmt::dms));
    Geodetic_Inverse_Solve_Result ri = gauss_lemma_solve(B1, L1, B2, L2, cgcs2000);

    std::println("S = {}, A12 = {}, A21 = {}",
                 ri.s,
                 ri.forward.toString(fmt::dms),
                 ri.backward.toString(fmt::dms));

    Geodetic_Forward_Solve_Result rf = gauss_lemma_solve(B1, L1, ri.s, ri.forward, cgcs2000);

    std::println("B2 = {}, L2 = {}, A21 = {}",
                 rf.lat.toString(fmt::dms),
                 rf.lon.toString(fmt::dms),
                 rf.backward.toString(fmt::dms));

    double
        dl = rad2sec(rf.lon.rad() - L2.rad()),
        db = rad2sec(rf.lat.rad() - B2.rad()),
        da = rad2sec(rf.backward.toRadian() - ri.backward.toRadian());
    std::println("dl = {}, db = {}, da = {}", dl, db, da);
}
