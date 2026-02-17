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
        int p_precision)
    {
        double unit =
            p_precision > 0 ? std::pow(0.1, p_precision) : 1.0;
        double dif = cut(p_sum - p_target, p_precision);
        unit = cut(unit, p_precision);
        bool dif_sign = std::signbit(dif);
        unit = dif_sign ? -unit : unit;
        int count = dif / unit;

        size_t idx = 0;

        if (eqApprox(dif, 0.0))
        {
            return;
        }

        if (dif_sign)
        {
            while (count-- > 0)
            {
                auto min = std::min_element(p_vec.begin(), p_vec.end());
                idx = std::distance(p_vec.begin(), min);
                double val = cut(p_vec.at(idx) - unit, p_precision);
                p_vec.at(idx) = val;
            }
        }
        else
        {
            while (count-- > 0)
            {
                auto max = std::max_element(p_vec.begin(), p_vec.end());
                idx = std::distance(p_vec.begin(), max);
                double val = cut(p_vec.at(idx) - unit, p_precision);
                p_vec.at(idx) = val;
            }
        }

        return;
    }

    double calcTolerance(
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

#if !(M_libga_with_impl)
extern bool closedElevAssert(const Adjust_Frame_Result &);
extern bool attachedElevAssert(const Adjust_Frame_Result &);
extern bool closedTraverseAssert(const Adjust_Frame_Result &);
extern bool connectingTraverseAssert(const Adjust_Frame_Result &);
#else
bool closedElevAssert(const Adjust_Frame_Result &);
bool attachedElevAssert(const Adjust_Frame_Result &);
bool closedTraverseAssert(const Adjust_Frame_Result &);
bool connectingTraverseAssert(const Adjust_Frame_Result &);
#endif

bool frameAssert(Route_Type p_rt, const Adjust_Frame_Result &p_afr)
{
    switch (p_rt)
    {
    case CLOSED_ELEV:
        return closedElevAssert(p_afr);
    case ATTACHED_ELEV:
        return attachedElevAssert(p_afr);
    case CLOSED_TRA:
        return closedTraverseAssert(p_afr);
    case CONNECTING_TRA:
        return connectingTraverseAssert(p_afr);
    default:
        return false;
    }
}

M_libga_end