#include <lga/Angle>

M_libga_begin

    namespace internal
{
    const double pi = std::numbers::pi;
    const std::array<double (*)(double), 3> angle_converters = {
        &deg2rad,
        &min2rad,
        &sec2rad};
    const double
        __deg2rad = pi / 180.0,
        __rad2deg = 180.0 / pi,
        __min2rad = __deg2rad / 60.0,
        __rad2min = __rad2deg * 60.0,
        __sec2rad = __deg2rad / 3600.0,
        __rad2sec = __rad2deg * 3600.0;
}

double sec2rad(double p_s) { return p_s * internal::__sec2rad; }
double min2rad(double p_m) { return p_m * internal::__min2rad; }
double deg2rad(double p_d) { return p_d * internal::__deg2rad; }
double rad2sec(double p_r) { return p_r * internal::__rad2sec; }
double rad2min(double p_r) { return p_r * internal::__rad2min; }
double rad2deg(double p_r) { return p_r * internal::__rad2deg; }

Angle::Angle(std::initializer_list<double> il)
    : m_radian(dms2rad(il.begin(), il.end())) {}

Angle Angle::fromSeconds(double p_second) { return Angle(sec2rad(p_second)); }
Angle Angle::fromMinutes(double p_m) { return Angle(min2rad(p_m)); }
Angle Angle::fromDegrees(double p_d) { return Angle(deg2rad(p_d)); }
Angle Angle::fromRadian(double p_rad) { return Angle(p_rad); }
Angle Angle::fromDMS(double p_d, double p_m, double p_s, bool p_sign)
{
    auto ang{Angle(
        deg2rad(std::abs(p_d)) +
        min2rad(std::abs(p_m)) +
        sec2rad(std::abs(p_s)))};
    ang.m_radian *= (p_sign ? -1 : 1);
    return ang;
}
double Angle::toSeconds() const { return rad2sec(m_radian); }
double Angle::toMinutes() const { return rad2min(m_radian); }
double Angle::toDegrees() const { return rad2deg(m_radian); }
double Angle::toRadian() const { return m_radian; }
double Angle::seconds() const
{
    return std::abs(std::fmod(rad2sec(m_radian), 60.0));
}
int Angle::minutes() const
{
    return std::abs(static_cast<int>(std::fmod(rad2sec(m_radian), 3600.0) / 60.0));
}
int Angle::degrees() const { return std::abs(static_cast<int>(rad2deg(m_radian))); }
std::tuple<int, int, double> Angle::toDMS() const
{
    return std::make_tuple(degrees(), minutes(), seconds());
}

double Angle::sin() const { return std::sin(m_radian); }
double Angle::cos() const { return std::cos(m_radian); }
double Angle::tan() const { return std::tan(m_radian); }
double Angle::sec() const { return 1.0 / cos(); }
double Angle::csc() const { return 1.0 / sin(); }
double Angle::cot() const { return 1.0 / tan(); }

Angle Angle::operator+(const Angle &p_other) const
{
    return Angle(m_radian + p_other.m_radian);
}
Angle Angle::operator-(const Angle &p_other) const
{
    return Angle(m_radian - p_other.m_radian);
}
Angle Angle::operator-() const
{
    return Angle(-m_radian);
}
Angle Angle::operator%(const Angle &p_other) const
{
    return Angle(std::fmod(m_radian, p_other.m_radian));
}
double Angle::operator/(const Angle &p_other) const
{
    return m_radian / p_other.m_radian;
}
Angle operator*(const Angle &p_ang, double p_scale)
{
    return Angle(p_ang.m_radian * p_scale);
}
Angle operator*(double p_scale, const Angle &p_ang)
{
    return Angle(p_ang.m_radian * p_scale);
}
Angle operator/(const Angle &p_ang, double p_scale)
{
    return Angle(p_ang.m_radian / p_scale);
}
Angle operator/(double p_scale, const Angle &p_ang)
{
    return Angle(p_ang.m_radian / p_scale);
}

bool Angle::withinAbs(const Angle &p_other, double p_threshold) const
{
    return std::abs(m_radian - p_other.m_radian) < std::abs(p_threshold);
}
bool Angle::eqApprox(const Angle &p_other, double p_threshold) const
{
    return this->withinAbs(p_other, p_threshold);
}
bool Angle::ltAbs(const Angle &p_other, double p_threshold) const
{
    return std::abs(m_radian) < std::abs(p_other.m_radian);
}
bool Angle::gtAbs(const Angle &p_other, double p_threshold) const
{
    return std::abs(m_radian) > std::abs(p_other.m_radian);
}
bool Angle::lt(const Angle &p_other) const
{
    return m_radian < p_other.m_radian;
}
bool Angle::gt(const Angle &p_other) const
{
    return m_radian > p_other.m_radian;
}

Angle Angle::norm() const
{
    double rad = std::abs(m_radian);
    rad = std::fmod(rad, 2 * internal::pi);
    return Angle(rad);
}
Angle Angle::normSym() const
{
    M_libga_not_impl("I don't know what it exactly means.");
    return this->norm() - Angle(internal::pi);
}
Angle Angle::abs() const
{
    return Angle(std::abs(m_radian));
}
Angle Angle::std() const
{
    double rad = m_radian;

    while (rad > 2 * internal::pi)
    {
        rad -= 2 * internal::pi;
    }

    while (rad < 0)
    {
        rad += 2 * internal::pi;
    }

    return Angle(rad);
}

Angle Angle::zero()
{
    return Angle(0.0);
}
Angle Angle::singlePi()
{
    return Angle(internal::pi);
}
Angle Angle::doublePi()
{
    return Angle(internal::pi * 2);
}

Angle Angle::fromString(std::string_view p_str, std::string_view p_pattern)
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

internal::Angle_Format_Wrapper
Angle::
    format(std::string_view p_fmt) const noexcept
{
    return internal::Angle_Format_Wrapper{
        .angle = *this,
        .fmt = p_fmt};
}

std::ostream &
internal::
operator<<(
    std::ostream &os,
    const Angle_Format_Wrapper &p_afw)
{
    os << p_afw.angle.toString(p_afw.fmt);
    return os;
}

double dms2rad(double p_d, double p_m, double p_s)
{
    std::array<double, 3> dms{p_d, p_m, p_s};
    return dms2rad(dms.begin(), dms.end());
}

M_libga_end