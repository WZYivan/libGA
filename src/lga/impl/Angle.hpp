/**
 * @file Angle.hpp
 * @author WZYivan (227006975@qq.com)
 * @brief Implement of compile time of `<lga/Angle>`
 * @version 0.1.0
 * @date 2026-02-08
 *
 * @copyright Copyright (c) 2026
 *
 */

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
    requires concepts::Iter_Like<Iter, double>
inline double dms2rad(Iter p_begin, Iter p_end)
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