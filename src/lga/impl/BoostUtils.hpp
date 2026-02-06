#ifndef M_libga_impl_boost_utils
#define M_libga_impl_boost_utils

#include <sstream>

#include <lga/BoostUtils>

M_libga_begin

    namespace internal
{
    template <typename Iter, typename Conv_Fn>
    inline std::string format2jsonArray(
        Iter p_begin,
        Iter p_end,
        Conv_Fn p_conv)
        requires std::invocable<Conv_Fn, Iter> &&
                 concepts::Can_Output_To_Ostream<decltype(p_conv(p_begin))>
    {
        std::ostringstream oss{};
        oss << "[";
        for (Iter iter = p_begin; iter != p_end; ++iter)
        {

            oss << p_conv(iter);

            if (iter + 1 != p_end)
            {
                oss << ", ";
            }
        }
        oss << "]";
        return oss.str();
    }

    template <typename Iter>
    inline std::string format2jsonArray(Iter p_begin, Iter p_end)
        requires concepts::Can_Output_To_Ostream<decltype(*p_begin)>
    {
        std::ostringstream oss{};
        oss << "[";
        for (Iter iter = p_begin; iter != p_end; ++iter)
        {
            oss << *iter;

            if (iter + 1 != p_end)
            {
                oss << ", ";
            }
        }
        oss << "]";
        return oss.str();
    }
}

template <typename Range>
inline std::string format2jsonArray(Range p_range)
    requires concepts::Can_Access_Begin_End<Range> &&
             concepts::Can_Output_To_Ostream<decltype(*(p_range.begin()))>
{
    return internal::format2jsonArray(p_range.begin(), p_range.end());
}

template <typename Range, typename Conv_Fn>
inline std::string format2jsonArray(
    Range p_range,
    Conv_Fn p_conv)
    requires concepts::Can_Access_Begin_End<Range> &&
             concepts::Can_Output_To_Ostream<decltype(*(p_range.begin()))> &&
             std::invocable<Conv_Fn, decltype(p_range.begin())> &&
             concepts::Can_Output_To_Ostream<decltype(p_conv(p_range.begin()))>
{
    return internal::format2jsonArray(p_range.begin(), p_range.end(), p_conv);
}
M_libga_end

#endif