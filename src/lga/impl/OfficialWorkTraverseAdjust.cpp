#include <algorithm>
#include <numeric>

#include <lga/OfficialWork>

M_libga_begin

    Adjust_Result
    closedTraverseAdjust(
        std::vector<Angle> &p_left_angle,
        std::vector<double> &p_distance,
        const Angle &p_angle_beg,
        double p_x_beg,
        double p_y_beg,
        int p_angle_correction_precision,
        int p_distance_correction_precision,
        std::function<double(double, double)> p_tolerence_fn)
{
    size_t size = p_left_angle.size();

    if (size + 1 != p_distance.size())
    {
        throw std::invalid_argument(
            std::format(
                "input data size: {} + 1 != {}",
                size,
                p_distance.size()));
    }

    Adjust_Result result;

    DataFrame &df = result.frame;
    DataFrameColumnIndices indices(size + 1);
    std::iota(indices.begin(), indices.end(), 0);
    df.load_index(std::move(indices));

    Angle left_angle_sum =
        std::accumulate(
            p_left_angle.begin(),
            p_left_angle.end(),
            Angle(0.0));

    std::vector<std::string> angle_sum_idx_col{
        "left angle",
        "angle correction",
        "corrected angle"};
    std::vector<std::string> distance_sum_idx_col{
        "distance",
        "delta x",
        "delta y",
        "delta x correction",
        "delta y correction",
        "corrected delta x",
        "corrected delta y"};

    DataFrame &info = result.info_frame;
    DataFrameColumnIndices info_indices(4);
    std::iota(info_indices.begin(), info_indices.end(), 0);
    info.load_index(std::move(info_indices));

    return result;
}

M_libga_end