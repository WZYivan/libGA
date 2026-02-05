#include <print>
#include <iostream>

#include <lga/Geodesy>

int main()
{
    if (true)
        for (int i = 1; i != 90; ++i)
        {
            lga::Longitude L_from(lga::dms2rad(115, 0, 0));
            lga::Latitude B_from(lga::dms2rad(i, 0, 0));

            lga::Geodetic_Coordinate geo_coord_from{
                .lat = B_from,
                .lon = L_from};

            lga::Gauss_Project_Coordinate gauss_proj_coord =
                lga::gauss_project(
                    geo_coord_from,
                    lga::param::geodesy.gauss_project_interval.six,
                    lga::cgcs2000);

            lga::Geodetic_Coordinate geo_coord_to =
                lga::gauss_project(
                    gauss_proj_coord,
                    lga::cgcs2000);

            std::println(
                "B_from = {} L_from = {}",
                B_from.toString(lga::fmt::dms),
                L_from.toString(lga::fmt::dms));
            std::println(
                "x = {} y = {} ZoneY = {}",
                gauss_proj_coord.x,
                gauss_proj_coord.y,
                lga::gauss_project.zoneY(gauss_proj_coord));
            std::println(
                "B_to = {} L_to = {}",
                geo_coord_to.lat.toString(lga::fmt::dms),
                geo_coord_to.lon.toString(lga::fmt::dms));
            std::println(
                "B_dif = {} L_dif = {}\n",
                lga::Angle(geo_coord_to.lat.rad() - B_from.rad()).toString(),
                lga::Angle(geo_coord_to.lon.rad() - L_from.rad()).toString());
        }

    std::println(
        "Bf = {:f} exptect {:f}",
        lga::meridianArcBottom(110642.229407, lga::cgcs2000).rad(),
        0.017464);
}