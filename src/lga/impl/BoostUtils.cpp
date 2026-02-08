#include <limits>
#include <ranges>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <lga/BoostUtils>

M_libga_begin

    double
    enforce2double(const boost::json::value &p_bst_j_val)
{
    const boost::json::value &val = p_bst_j_val;

    auto dbl = val.try_as_double();
    if (dbl.has_value())
    {
        return dbl.value();
    }

    auto i64 = val.try_as_int64();
    if (i64.has_value())
    {
        return i64.value();
    }

    auto ui64 = val.try_as_uint64();
    if (ui64.has_value())
    {
        return ui64.value();
    }

    return std::numeric_limits<double>::quiet_NaN();
}

Angle enforce2angle(const boost::json::array &p_bst_j_arr)
{
    const boost::json::array &arr = p_bst_j_arr;

    auto dms{
        arr |
        std::views::transform(
            [](const boost::json::value &val) -> double
            {
                return enforce2double(val);
            })};

    return Angle(dms2rad(dms.begin(), dms.end()));
}

void preprocessJsonStr(
    std::string &p_str,
    const boost::json::parse_options &p_opt)
{
    boost::replace_all(p_str, "+", "");

    if (!p_opt.allow_infinity_and_nan)
    {
        boost::replace_all(p_str, "nan", "null");
        boost::replace_all(p_str, "NaN", "null");
    }
    else
    {
        boost::replace_all(p_str, "nan", "NaN");
    }
}

M_libga_end