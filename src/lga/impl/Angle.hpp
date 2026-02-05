#ifndef M_libga_impl_angle
#define M_libga_impl_angle

#include <lga/Angle>

M_libga_begin

    namespace internal
{

    constexpr double
        __deg2rad = pi / 180.0,
        __rad2deg = 180.0 / pi,
        __min2rad = __deg2rad / 60.0,
        __rad2min = __rad2deg * 60.0,
        __sec2rad = __deg2rad / 3600.0,
        __rad2sec = __rad2deg * 3600.0;
}

constexpr double sec2rad(double p_s) { return p_s * internal::__sec2rad; }
constexpr double min2rad(double p_m) { return p_m * internal::__min2rad; }
constexpr double deg2rad(double p_d) { return p_d * internal::__deg2rad; }
constexpr double rad2sec(double p_r) { return p_r * internal::__rad2sec; }
constexpr double rad2min(double p_r) { return p_r * internal::__rad2min; }
constexpr double rad2deg(double p_r) { return p_r * internal::__rad2deg; }

constexpr Angle::Angle(std::initializer_list<double> il)
    : m_radian(dms2rad(il.begin(), il.end())) {}

constexpr Angle Angle::fromSeconds(double p_second) { return Angle(sec2rad(p_second)); }
constexpr Angle Angle::fromMinutes(double p_m) { return Angle(min2rad(p_m)); }
constexpr Angle Angle::fromDegrees(double p_d) { return Angle(deg2rad(p_d)); }
constexpr Angle Angle::fromRadian(double p_rad) { return Angle(p_rad); }
constexpr Angle Angle::fromDMS(double p_d, double p_m, double p_s, bool p_sign)
{
    auto ang{Angle(
        deg2rad(std::abs(p_d)) +
        min2rad(std::abs(p_m)) +
        sec2rad(std::abs(p_s)))};
    ang.m_radian *= (p_sign ? -1 : 1);
    return ang;
}
constexpr double Angle::toSeconds() const { return rad2sec(m_radian); }
constexpr double Angle::toMinutes() const { return rad2min(m_radian); }
constexpr double Angle::toDegrees() const { return rad2deg(m_radian); }
constexpr double Angle::toRadian() const { return m_radian; }
constexpr double Angle::seconds() const
{
    return std::abs(std::fmod(rad2sec(m_radian), 60.0));
}
constexpr int Angle::minutes() const
{
    return std::abs(static_cast<int>(std::fmod(rad2sec(m_radian), 3600.0) / 60.0));
}
constexpr int Angle::degrees() const { return std::abs(static_cast<int>(rad2deg(m_radian))); }
constexpr std::tuple<int, int, double> Angle::toDMS() const
{
    return std::make_tuple(degrees(), minutes(), seconds());
}

constexpr double Angle::sin() const { return std::sin(m_radian); }
constexpr double Angle::cos() const { return std::cos(m_radian); }
constexpr double Angle::tan() const { return std::tan(m_radian); }
constexpr double Angle::sec() const { return 1.0 / cos(); }
constexpr double Angle::csc() const { return 1.0 / sin(); }
constexpr double Angle::cot() const { return 1.0 / tan(); }

constexpr Angle Angle::operator+(const Angle &p_other) const
{
    return Angle(m_radian + p_other.m_radian);
}
constexpr Angle Angle::operator-(const Angle &p_other) const
{
    return Angle(m_radian - p_other.m_radian);
}
constexpr Angle Angle::operator-() const
{
    return Angle(-m_radian);
}
constexpr Angle Angle::operator%(const Angle &p_other) const
{
    return Angle(std::fmod(m_radian, p_other.m_radian));
}
constexpr double Angle::operator/(const Angle &p_other) const
{
    return m_radian / p_other.m_radian;
}
constexpr Angle operator*(const Angle &p_ang, double p_scale)
{
    return Angle(p_ang.m_radian * p_scale);
}
constexpr Angle operator*(double p_scale, const Angle &p_ang)
{
    return Angle(p_ang.m_radian * p_scale);
}
constexpr Angle operator/(const Angle &p_ang, double p_scale)
{
    return Angle(p_ang.m_radian / p_scale);
}
constexpr Angle operator/(double p_scale, const Angle &p_ang)
{
    return Angle(p_ang.m_radian / p_scale);
}

constexpr bool Angle::withinAbs(const Angle &p_other, double p_threshold) const
{
    return std::abs(m_radian - p_other.m_radian) < std::abs(p_threshold);
}
constexpr bool Angle::eqApprox(const Angle &p_other, double p_threshold) const
{
    return this->withinAbs(p_other, p_threshold);
}
constexpr bool Angle::ltAbs(const Angle &p_other, double p_threshold) const
{
    return std::abs(m_radian) < std::abs(p_other.m_radian);
}
constexpr bool Angle::gtAbs(const Angle &p_other, double p_threshold) const
{
    return std::abs(m_radian) > std::abs(p_other.m_radian);
}
constexpr bool Angle::lt(const Angle &p_other) const
{
    return m_radian < p_other.m_radian;
}
constexpr bool Angle::gt(const Angle &p_other) const
{
    return m_radian > p_other.m_radian;
}

constexpr Angle Angle::norm() const
{
    double rad = std::abs(m_radian);
    rad = std::fmod(rad, 2 * pi);
    return Angle(rad);
}
constexpr Angle Angle::normSym() const
{
    M_libga_not_impl("I don't know what it exactly means.");
    return this->norm() - Angle(pi);
}
constexpr Angle Angle::abs() const
{
    return Angle(std::abs(m_radian));
}

constexpr Angle Angle::zero()
{
    return Angle(0.0);
}
constexpr Angle Angle::singlePi()
{
    return Angle(pi);
}
constexpr Angle Angle::doublePi()
{
    return Angle(pi * 2);
}

constexpr Angle Angle::fromString(std::string_view p_str, std::string_view p_pattern)
{
    std::regex pattern{std::string(p_pattern)};
    std::smatch match;
    std::string str(p_str);

    if (!std::regex_match(str, match, pattern))
    {
        throw std::invalid_argument(std::format("Input string does not match the expected pattern: {}", p_pattern));
    }

    if (match.size() < 4)
    {
        throw std::runtime_error("Pattern does not contain expected capture groups");
    }

    std::string sign_str = match[1].str();
    bool sign = (sign_str == "-");

    int degrees = std::stoi(match[2].str());
    int minutes = std::stoi(match[3].str());
    const auto sec_str = match[4].str();
    int sec_str_size = sec_str.size();
    double seconds = (std::stod(sec_str)) / (sec_str_size <= 2 ? 1.0 : std::pow(10, sec_str_size - 2));

    return Angle::fromDMS(degrees, minutes, seconds, sign);
}

std::string Angle::toString(std::string_view p_format) const
{
    int d = degrees(), m = minutes();
    double s = seconds();
    return std::vformat(p_format,
                        std::make_format_args(m_radian > 0 ? "+" : "-",
                                              d,
                                              m,
                                              s));
}

M_libga_end

#endif