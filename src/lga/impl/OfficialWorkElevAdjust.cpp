#include <numeric>
#include <algorithm>

#include <DataFrame/DataFrameFinancialVisitors.h>

#include <lga/OfficialWork>

M_libga_begin

    DataFrame
    closedElevAdjust(
        std::vector<double> &p_distances,
        std::vector<double> &p_diff,
        double p_beg,
        int p_correction_precision,
        std::function<double(double)> p_tolerence_fn)
{
    size_t size = p_distances.size();

    if (size != p_diff.size())
    {
        throw std::invalid_argument(
            std::format(
                "size {} != {}",
                size,
                p_diff.size()));
    }

    double closure = std::accumulate(p_diff.begin(), p_diff.end(), 0.0);
    double tolerence = internal::calcTolerence(p_distances, p_tolerence_fn);

    DataFrameColumnIndices indices(size + 1);
    std::iota(indices.begin(), indices.end(), 0);

    DataFrame df;
    df.load_index(std::move(indices));
    df.load_column("distance", std::move(p_distances));
    df.load_column("diff", std::move(p_diff));

    hmdf::SumVisitor<double> sum_v(true);
    df.visit<double>("distance", sum_v);
    double distance_sum = sum_v.get_result();
    df.visit<double>("diff", sum_v);
    double diff_sum = sum_v.get_result();

    double correction_sum = cut(
        -1 * diff_sum,
        p_correction_precision);
    double mean_correction = cut(
        correction_sum / (distance_sum),
        p_correction_precision);

    std::vector<double> corrections(size);
    const auto &distances =
        df.get_column<double>("distance");
    for (auto idx = 0uz; idx != size; ++idx)
    {
        corrections.at(idx) =
            cut(
                distances.at(idx) * mean_correction,
                p_correction_precision);
    }
    internal::adjustSumToTarget(
        corrections,
        std::accumulate(
            corrections.begin(),
            corrections.end(),
            0.0),
        correction_sum,
        p_correction_precision);

    std::vector<double> corrected_diff(size);
    const auto &diff = df.get_column<double>("diff");
    for (auto idx = 0uz; idx != size; ++idx)
    {
        corrected_diff.at(idx) =
            diff.at(idx) + corrections.at(idx);
    }

    std::vector<double> elev(size + 1);
    elev.at(0) = p_beg;
    for (auto idx = 0uz; idx != size; ++idx)
    {
        elev.at(idx + 1) =
            elev.at(idx) + corrected_diff.at(idx);
    }

    df.load_column("correction", std::move(corrections));
    df.load_column("corrected diff", std::move(corrected_diff));
    df.visit<double>("corrected diff", sum_v);
    double corrected_dif_sum = zeroOr(sum_v.get_result());
    df.load_column("elev", std::move(elev));

    std::vector<std::string> sum_idx_col{
        "distances", "diff", "correction", "corrected diff"};
    std::vector<double> sum_col{
        distance_sum, diff_sum, correction_sum, corrected_dif_sum};
    df.load_column("sum of(name)", std::move(sum_idx_col));
    df.load_column("sum of(val)", std::move(sum_col));

    std::vector<std::string> info_idx_col{
        "closure", "tolerence:mm", "mean correction",
        "distance => km"};
    std::vector<double> info_col{
        closure, tolerence, mean_correction};
    df.load_column("info(name)", std::move(info_idx_col));
    df.load_column("info(val)", std::move(info_col));

    return df;
}

DataFrame
attachedElevAdjust(
    std::vector<double> &p_distances,
    std::vector<double> &p_diff,
    double p_beg,
    double p_end,
    int p_correction_precision,
    std::function<double(double)> p_tolerence_fn)
{
    size_t size = p_distances.size();

    if (size != p_diff.size())
    {
        throw std::invalid_argument(
            std::format(
                "size {} != {}",
                size,
                p_diff.size()));
    }

    double closure = std::accumulate(p_diff.begin(), p_diff.end(), 0.0) - (p_end - p_beg);
    double tolerence = internal::calcTolerence(p_distances, p_tolerence_fn);

    DataFrameColumnIndices indices(size + 1);
    std::iota(indices.begin(), indices.end(), 0);

    DataFrame df;
    df.load_index(std::move(indices));
    df.load_column("distance", std::move(p_distances));
    df.load_column("diff", std::move(p_diff));

    hmdf::SumVisitor<double> sum_v(true);
    df.visit<double>("distance", sum_v);
    double distance_sum = sum_v.get_result();
    df.visit<double>("diff", sum_v);
    double diff_sum = sum_v.get_result();

    double correction_sum = cut(
        -1 * closure,
        p_correction_precision);
    double mean_correction = cut(
        correction_sum / (distance_sum),
        p_correction_precision);

    std::vector<double> corrections(size);
    const auto &distances =
        df.get_column<double>("distance");
    for (auto idx = 0uz; idx != size; ++idx)
    {
        corrections.at(idx) =
            cut(
                distances.at(idx) * mean_correction,
                p_correction_precision);
    }
    internal::adjustSumToTarget(
        corrections,
        std::accumulate(
            corrections.begin(),
            corrections.end(),
            0.0),
        correction_sum,
        p_correction_precision);

    std::vector<double> corrected_diff(size);
    const auto &diff = df.get_column<double>("diff");
    for (auto idx = 0uz; idx != size; ++idx)
    {
        corrected_diff.at(idx) =
            diff.at(idx) + corrections.at(idx);
    }

    std::vector<double> elev(size + 1);
    elev.at(0) = p_beg;
    for (auto idx = 0uz; idx != size; ++idx)
    {
        elev.at(idx + 1) =
            elev.at(idx) + corrected_diff.at(idx);
    }

    df.load_column("correction", std::move(corrections));
    df.load_column("corrected diff", std::move(corrected_diff));
    df.visit<double>("corrected diff", sum_v);
    double corrected_dif_sum = zeroOr(sum_v.get_result());
    df.load_column("elev", std::move(elev));

    std::vector<std::string> sum_idx_col{
        "distances", "diff", "correction", "corrected diff"};
    std::vector<double> sum_col{
        distance_sum, diff_sum, correction_sum, corrected_dif_sum};
    df.load_column("sum of(name)", std::move(sum_idx_col));
    df.load_column("sum of(val)", std::move(sum_col));

    std::vector<std::string> info_idx_col{
        "closure", "tolerence:mm", "mean correction", "expect target elev",
        "distance => km"};
    std::vector<double> info_col{
        closure, tolerence, mean_correction, p_end};
    df.load_column("info(name)", std::move(info_idx_col));
    df.load_column("info(val)", std::move(info_col));

    return df;
}
M_libga_end