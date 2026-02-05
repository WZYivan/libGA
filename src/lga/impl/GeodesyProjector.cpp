#include <lga/Geodesy>

M_libga_begin

    namespace internal
{
    int zone3(Longitude p_lon)
    {
        double
            rad_d3 = deg2rad(3.0),
            rad_d1_5 = rad_d3 / 2.0;
        return int(p_lon.rad() / rad_d3) + (std::fmod(
                                                p_lon.rad(),
                                                rad_d3) > rad_d1_5
                                                ? 1
                                                : 0);
    }
    int zone6(Longitude p_lon)
    {
        return int(p_lon.rad() / deg2rad(6.0)) + 1;
    }

    Longitude centerMeridian3(int p_zone)
    {
        return Longitude(deg2rad(3 * p_zone));
    }

    Longitude centerMeridian6(int p_zone)
    {
        return Longitude(deg2rad(6 * p_zone - 3));
    }
}

int Gauss_Projector::
    zone(const Longitude &p_lon, double p_interval) const
{
    if (p_interval == 3.0)
    {
        return internal::zone3(p_lon);
    }
    else if (p_interval == 6.0)
    {
        return internal::zone6(p_lon);
    }
    else
    {
        throw std::invalid_argument(std::format(
            "unknown gauss interval {:f}", p_interval));
    }
}

Longitude
Gauss_Projector::
    centerMeridian(const Longitude &p_lon, double p_interval) const
{
    return centerMeridian(zone(p_lon, p_interval), p_interval);
}

Longitude
Gauss_Projector::
    centerMeridian(int p_zone, double p_interval) const
{
    if (p_interval == 3.0)
    {
        return internal::centerMeridian3(p_zone);
    }
    else if (p_interval == 6.0)
    {
        return internal::centerMeridian6(p_zone);
    }
    else
    {
        throw std::invalid_argument(std::format(
            "unknown gauss interval {:f}", p_interval));
    }
}

double
Gauss_Projector::
    zoneY(const Gauss_Project_Coordinate &p_gc) const noexcept
{
    return p_gc.y + 500'000.0 + p_gc.zone * 100'000'0;
}

Gauss_Project_Coordinate
Gauss_Projector::
    forward(
        const Geodetic_Coordinate &p_gc,
        double p_interval,
        const Ellipsoid &p_ellipsoid,
        int p_zone) const
{
    const Longitude &L = p_gc.lon;
    const Latitude &B = p_gc.lat;
    Ellipsoid_Geometry_Latitude_Aux glc(B, p_ellipsoid);

    double t = glc.t,
           t2 = std::pow(t, 2),
           t4 = std::pow(t, 4),
           n2 = glc.nu_2,
           n4 = std::pow(n2, 2);
    double p = rho2,
           p2 = std::pow(p, 2),
           p3 = std::pow(p, 3),
           p4 = std::pow(p, 4),
           p5 = std::pow(p, 5),
           p6 = std::pow(p, 6);
    double sinB = B.sin(),
           cosB = B.cos(),
           cosBp3 = std::pow(cosB, 3),
           cosBp5 = std::pow(cosB, 5);
    int zone = p_zone == -1 ? this->zone(L, p_interval) : p_zone;
    double l_c = centerMeridian(zone, p_interval).rad(),
           l_c_s = rad2sec(l_c) /*seconds below*/,
           l_s = rad2sec(L.rad()),
           dl_s = l_s - l_c_s,
           l = dl_s,
           l2 = std::pow(dl_s, 2),
           l3 = std::pow(dl_s, 3),
           l4 = std::pow(dl_s, 4),
           l5 = std::pow(dl_s, 5),
           l6 = std::pow(dl_s, 6);
    auto pcr = principleCurvatureRadius(B, p_ellipsoid);
    double N = pcr.n;
    double X = meridianArcLength(B, p_ellipsoid);

#if (M_libga_debug)
    std::println(
        "X = {:f}, N = {:f}, l = {:f}",
        X,
        N,
        l);
#endif

    double x = X +
               N / (2 * p2) * sinB * cosB * l2 +
               N / (24 * p4) * sinB * cosBp3 * (5 - t2 + 9 * n2 + 4 * n4) * l4 +
               N / (720 * p6) * sinB * cosBp5 * (61 - 58 * t2 + t4) * l6;
    double y = N / p * cosB * l +
               N / (6 * p3) * cosBp3 * (1 - t2 + n2) * l3 +
               N / (120 * p5) * cosBp5 * (5 - 18 * t2 + t4 + 14 * n2 - 58 * n2 * t2) * l5;
    return Gauss_Project_Coordinate{
        .interval = p_interval,
        .x = x,
        .y = y,
        .zone = zone};
}

Geodetic_Coordinate
Gauss_Projector::
    inverse(
        const Gauss_Project_Coordinate &p_gpc,
        const Ellipsoid &p_ellipsoid) const
{
    double x = p_gpc.x, y = p_gpc.y;
    double zone = p_gpc.zone;
    Latitude Bf = meridianArcBottom(x, p_ellipsoid);
    auto [Mf, Nf] = principleCurvatureRadius(Bf, p_ellipsoid);
    Ellipsoid_Geometry_Latitude_Aux glc(Bf, p_ellipsoid);

    double tf = glc.t, nf2 = glc.nu_2;
    double tf2 = std::pow(tf, 2),
           tf4 = std::pow(tf, 4),
           nf4 = std::pow(nf2, 2);
    double Nf3 = std::pow(Nf, 3),
           Nf5 = std::pow(Nf, 5);
    double cosBf = Bf.cos();
    double y2 = std::pow(y, 2),
           y3 = std::pow(y, 3),
           y4 = std::pow(y, 4),
           y5 = std::pow(y, 5),
           y6 = std::pow(y, 6);

    double B = Bf.rad() -
               tf / (2.0 * Mf * Nf) * y2 +
               tf / (24 * Mf * Nf3) * (5 + 3 * tf2 + nf2 - 9 * nf2 * tf2) * y4 -
               tf / (720 * Mf * Nf5) * (61 + 90 * tf2 + 45 * tf4) * y6;
    double dl = 1.0 / (Nf * cosBf) * y -
                1.0 / (6.0 * Nf3 * cosBf) * (1 + 2 * tf2 + nf2) * y3 +
                1.0 / (120 * Nf5 * cosBf) * (5 + 28 * tf2 + 24 * tf4 + 6 * nf2 + 8 * nf2 * tf2) * y5;
    Longitude Lc = centerMeridian(zone, p_gpc.interval);

#if (M_libga_debug)
    std::println(
        "x = {:f}, Bf Rad = {:f}, Mf = {:f}, Nf = {:f}",
        x,
        Bf.rad(),
        Mf,
        Nf);
#endif

    return Geodetic_Coordinate{
        .lat = Latitude(B),
        .lon = Longitude(Lc.rad() + dl)};
}

Gauss_Project_Coordinate
Gauss_Projector::
operator()(
    const Geodetic_Coordinate &p_gc,
    double p_interval,
    const Ellipsoid &p_ellipsoid,
    int p_zone) const
{
    return forward(p_gc, p_interval, p_ellipsoid, p_zone);
}

Geodetic_Coordinate
Gauss_Projector::
operator()(
    const Gauss_Project_Coordinate &p_gpc,
    const Ellipsoid &p_ellipsoid) const
{
    return inverse(p_gpc, p_ellipsoid);
}

Gauss_Project_Coordinate
Gauss_Projector::
    modifyZone(
        const Gauss_Project_Coordinate &p_src,
        int p_tar_zone,
        const Ellipsoid &p_ellipsoid) const
{
    return forward(
        inverse(
            p_src,
            p_ellipsoid),
        p_tar_zone,
        p_ellipsoid);
}

const Gauss_Projector gauss_project{};

M_libga_end