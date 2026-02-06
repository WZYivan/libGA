#ifndef M_libga_impl_angle
#define M_libga_impl_angle

#include <lga/Angle>

M_libga_begin

    inline std::ostream &
    operator<<(std::ostream &os, const Angle &angle)
{
    os << angle.toString(config::angle.output_fmt);
    return os;
}

template <typename Iter>
inline double dms2rad(Iter p_begin, Iter p_end)
    requires std::convertible_to<decltype(*p_begin), double>
{
    size_t size = std::distance(p_begin, p_end);
    if (!(size >= 0 && size <= 3))
    {
        throw std::range_error("dms range size must be [1,3]");
    }
    if (size == 0)
    {
        return 0.0;
    }

    double rad{0.0};
    auto converter_iter = internal::angle_converters.begin();
    bool sign = std::signbit(*p_begin);

    while (size-- != 0)
    {
        rad += (*(converter_iter++))(std::abs(*(p_begin++)));
    }

    return rad * (sign ? -1 : 1);
}

M_libga_end

#endif