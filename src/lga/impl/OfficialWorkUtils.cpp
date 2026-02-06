#include <lga/OfficialWork>

M_libga_begin

    namespace param
{
    const Official_Work official_work = {};
}

namespace internal
{
    void adjustSumToTarget(
        std::vector<double> &p_vec,
        double p_sum,
        double p_target,
        int p_precesion)
    {
        double unit =
            p_precesion > 0 ? std::pow(0.1, p_precesion) : 1.0;
        double dif = cut(p_sum - p_target, p_precesion);
        unit = cut(unit, p_precesion);
        bool dif_sign = std::signbit(dif);
        unit = dif_sign ? -unit : unit;
        int count = dif / unit;

        while (count-- > 0)
        {
            size_t idx = 0;

            if (dif_sign)
            {
                auto min = std::min_element(p_vec.begin(), p_vec.end());
                idx = std::distance(p_vec.begin(), min);
            }
            else
            {
                auto max = std::max_element(p_vec.begin(), p_vec.end());
                idx = std::distance(p_vec.begin(), max);
            }

            p_vec.at(idx) = cut(p_vec.at(idx) - unit, p_precesion);
        }
    }

    double calcTolerence(
        const std::vector<double> &p_vec,
        std::function<double(double)> p_fn)
    {
        return p_fn(
            std::accumulate(
                p_vec.begin(),
                p_vec.end(),
                0.0));
    }

}

M_libga_end