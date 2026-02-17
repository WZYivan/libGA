#include <algorithm>
#include <numeric>

#include <lga/OfficialWork>

M_libga_begin

    Adjust_Frame_Result
    closedTraverseAdjust(
        std::vector<Angle> &p_left_angle,
        std::vector<double> &p_distance,
        const Angle &p_angle_beg,
        double p_x_beg,
        double p_y_beg,
        int p_angle_correction_precision,
        int p_distance_correction_precision,
        std::function<double(double, double)> p_tolerance_fn)
{
    size_t size = p_left_angle.size();

    if (size != p_distance.size())
    {
        throw std::invalid_argument(
            std::format(
                "input data size: {} != {}",
                size,
                p_distance.size()));
    }

    Adjust_Frame_Result result;

    DataFrame &df = result.frame;
    DataFrameColumnIndices indices(size + 1);
    std::iota(indices.begin(), indices.end(), 0);
    df.load_index(std::move(indices));

    Angle left_angle_sum =
        std::accumulate(
            p_left_angle.begin(),
            p_left_angle.end(),
            Angle(0.0));
    double distance_sum =
        std::accumulate(
            p_distance.begin(),
            p_distance.end(),
            0.0);
    double f_beta_expect = (size - 2) * 180;
    double f_beta = cut(
        left_angle_sum.toSeconds() - f_beta_expect * 3600,
        p_angle_correction_precision);
    double tolerance = p_tolerance_fn(size, distance_sum);
    std::vector<double> angle_correction(
        size,
        -cut(
            f_beta / size,
            p_angle_correction_precision));

    double angle_correction_sum =
        std::accumulate(
            angle_correction.begin(),
            angle_correction.end(),
            0.0);
    internal::adjustSumToTarget(
        angle_correction,
        angle_correction_sum,
        -f_beta,
        p_angle_correction_precision);

    std::vector<Angle> corrected_angle(size);
    for (size_t i = 0uz; i != size; ++i)
    {
        corrected_angle.at(i) =
            p_left_angle.at(i) +
            Angle::fromSeconds(angle_correction.at(i));
    }
    Angle corrected_angle_sum =
        std::accumulate(
            corrected_angle.begin(),
            corrected_angle.end(),
            Angle(0.0));

    std::vector<Angle> azimuth(size + 1);
    azimuth.at(0) = p_angle_beg;
    for (size_t i = 0uz; i != size; ++i)
    {
        azimuth.at(i + 1) =
            (azimuth.at(i) +
             corrected_angle.at(i) -
             Angle::singlePi())
                .std();
    }

    std::vector<double>
        delta_x(size),
        delta_y(size),
        correction_delta_x(size),
        correction_delta_y(size),
        corrected_delta_x(size),
        corrected_delta_y(size),
        x(size + 1),
        y(size + 1);
    x.at(0) = p_x_beg;
    y.at(0) = p_y_beg;

    for (size_t i = 0uz; i != size; ++i)
    {
        delta_x.at(i) =
            cut(
                p_distance.at(i) * azimuth.at(i).cos(),
                p_distance_correction_precision);
        delta_y.at(i) =
            cut(
                p_distance.at(i) * azimuth.at(i).sin(),
                p_distance_correction_precision);
    }

    double f_x = std::accumulate(delta_x.begin(), delta_x.end(), 0.0);
    double f_y = std::accumulate(delta_y.begin(), delta_y.end(), 0.0);
    double f =
        cut(
            std::sqrt(std::pow(f_x, 2) + std::pow(f_y, 2)), p_distance_correction_precision);
    double Kinv = std::floor(distance_sum / f);

    for (size_t i = 0uz; i != size; ++i)
    {
        correction_delta_x.at(i) =
            cut(
                p_distance.at(i) / distance_sum * -f_x,
                p_distance_correction_precision);
        correction_delta_y.at(i) =
            cut(
                p_distance.at(i) / distance_sum * -f_y,
                p_distance_correction_precision);
    }
    double correction_delta_x_sum =
        std::accumulate(
            correction_delta_x.begin(),
            correction_delta_x.end(),
            0.0);
    double correction_delta_y_sum =
        std::accumulate(
            correction_delta_y.begin(),
            correction_delta_y.end(),
            0.0);
    internal::adjustSumToTarget(
        correction_delta_x,
        correction_delta_x_sum,
        -f_x,
        p_distance_correction_precision);
    internal::adjustSumToTarget(
        correction_delta_y,
        correction_delta_y_sum,
        -f_y,
        p_distance_correction_precision);
    correction_delta_x_sum =
        std::accumulate(
            correction_delta_x.begin(),
            correction_delta_x.end(),
            0.0);
    correction_delta_y_sum =
        std::accumulate(
            correction_delta_y.begin(),
            correction_delta_y.end(),
            0.0);

    for (size_t i = 0uz; i != size; ++i)
    {
        corrected_delta_x.at(i) =
            cut(
                delta_x.at(i) + correction_delta_x.at(i),
                p_distance_correction_precision);
        corrected_delta_y.at(i) =
            cut(
                delta_y.at(i) + correction_delta_y.at(i),
                p_distance_correction_precision);
    }
    double corrected_delta_x_sum =
        std::accumulate(
            corrected_delta_x.begin(),
            corrected_delta_x.end(),
            0.0);
    double corrected_delta_y_sum =
        std::accumulate(
            corrected_delta_y.begin(),
            corrected_delta_y.end(),
            0.0);

    for (size_t i = 0uz; i != size; ++i)
    {
        x.at(i + 1) =
            cut(
                x.at(i) + corrected_delta_x.at(i),
                p_distance_correction_precision);
        y.at(i + 1) =
            cut(
                y.at(i) + corrected_delta_y.at(i),
                p_distance_correction_precision);
    }

    df.load_column("left angle", std::move(p_left_angle));
    df.load_column("angle correction", std::move(angle_correction));
    df.load_column("corrected angle", std::move(corrected_angle));
    df.load_column("azimuth", std::move(azimuth));
    df.load_column("distance", std::move(p_distance));
    df.load_column("delta x", std::move(delta_x));
    df.load_column("delta y", std::move(delta_y));
    df.load_column("correction of delta x", std::move(correction_delta_x));
    df.load_column("correction of delta y", std::move(correction_delta_y));
    df.load_column("corrected delta x", std::move(corrected_delta_x));
    df.load_column("corrected delta y", std::move(corrected_delta_y));
    df.load_column("x", std::move(x));
    df.load_column("y", std::move(y));

    std::vector<std::string>
        angle_sum_name_col{
            "left angle",
            "angle correction",
            "corrected angle"};
    std::vector<Angle> angle_sum_val_col{
        left_angle_sum,
        Angle::fromSeconds(angle_correction_sum),
        corrected_angle_sum};
    std::vector<std::string> distance_sum_name_col{
        "distance",
        "delta x",
        "delta y",
        "delta x correction",
        "delta y correction",
        "corrected delta x",
        "corrected delta y"};
    std::vector<double> distance_sum_val_col{
        distance_sum,
        f_x,
        f_y,
        correction_delta_x_sum,
        correction_delta_y_sum,
        corrected_delta_x_sum,
        corrected_delta_y_sum};
    std::vector<std::string> aux_info_name_col{
        "f beta",
        "f beta expect",
        "tolerance",
        "f x",
        "f y",
        "f",
        "1/K"};
    std::vector<double> aux_info_val_col{
        f_beta,
        f_beta_expect,
        tolerance,
        f_x,
        f_y,
        f,
        Kinv};
    std::vector<std::string> unit_name_col{
        "angle correction",
        "f beta",
        "f beta expect",
        "tolerance",
        "K",
        "default"};
    std::vector<std::string> unit_val_col{
        "second",
        "second",
        "degree",
        "mm",
        "N/A",
        "m"};

    DataFrame &info = result.info_frame;
    DataFrameColumnIndices info_indices(7);
    std::iota(info_indices.begin(), info_indices.end(), 0);
    info.load_index(std::move(info_indices));
    info.load_column("sum of angle(name)", std::move(angle_sum_name_col));
    info.load_column("sum of angle(val)", std::move(angle_sum_val_col));
    info.load_column("sum of distance(name)", std::move(distance_sum_name_col));
    info.load_column("sum of distance(val)", std::move(distance_sum_val_col));
    info.load_column("aux info(name)", std::move(aux_info_name_col));
    info.load_column("aux info(val)", std::move(aux_info_val_col));
    info.load_column("unit(name)", std::move(unit_name_col));
    info.load_column("unit(val)", std::move(unit_val_col));

    return result;
}

Adjust_Frame_Result
connectingTraverseAdjust(
    std::vector<Angle> &p_left_angle,
    std::vector<double> &p_distance,
    const Angle &p_angle_beg,
    const Angle &p_angle_end,
    double p_x_beg,
    double p_y_beg,
    double p_x_end,
    double p_y_end,
    int p_angle_correction_precision,
    int p_distance_correction_precision,
    std::function<double(double, double)> p_tolerance_fn)
{
    size_t size = p_left_angle.size();

    if (size - 1 != p_distance.size())
    {
        throw std::invalid_argument(
            std::format(
                "input data size: {} - 1 != {}",
                size,
                p_distance.size()));
    }

    Adjust_Frame_Result result;

    DataFrame &df = result.frame;
    DataFrameColumnIndices indices(size + 1);
    std::iota(indices.begin(), indices.end(), 0);
    df.load_index(std::move(indices));

    Angle left_angle_sum =
        std::accumulate(
            p_left_angle.begin(),
            p_left_angle.end(),
            Angle(0.0));
    double distance_sum =
        std::accumulate(
            p_distance.begin(),
            p_distance.end(),
            0.0);
    double f_beta_expect = size * 180 +
                           (p_angle_end.toDegrees() -
                            p_angle_beg.toDegrees());
    double f_beta = cut(
        left_angle_sum.toSeconds() - f_beta_expect * 3600,
        p_angle_correction_precision);
    double tolerance = p_tolerance_fn(size, distance_sum);
    std::vector<double> angle_correction(
        size,
        -cut(
            f_beta / size,
            p_angle_correction_precision));

    double angle_correction_sum =
        std::accumulate(
            angle_correction.begin(),
            angle_correction.end(),
            0.0);
    internal::adjustSumToTarget(
        angle_correction,
        angle_correction_sum,
        -f_beta,
        p_angle_correction_precision);

    std::vector<Angle> corrected_angle(size);
    for (size_t i = 0uz; i != size; ++i)
    {
        corrected_angle.at(i) =
            p_left_angle.at(i) +
            Angle::fromSeconds(angle_correction.at(i));
    }
    Angle corrected_angle_sum =
        std::accumulate(
            corrected_angle.begin(),
            corrected_angle.end(),
            Angle(0.0));

    std::vector<Angle> azimuth(size + 1);
    azimuth.at(0) = p_angle_beg;
    for (size_t i = 0uz; i != size; ++i)
    {
        azimuth.at(i + 1) =
            (azimuth.at(i) +
             corrected_angle.at(i) -
             Angle::singlePi())
                .std();
    }

    std::vector<double>
        delta_x(size - 1),
        delta_y(size - 1),
        correction_delta_x(size - 1),
        correction_delta_y(size - 1),
        corrected_delta_x(size - 1),
        corrected_delta_y(size - 1),
        x(size),
        y(size);
    x.at(0) = p_x_beg;
    y.at(0) = p_y_beg;

    for (size_t i = 0uz; i != size - 1; ++i)
    {
        delta_x.at(i) =
            cut(
                p_distance.at(i) * azimuth.at(i + 1).cos(),
                p_distance_correction_precision);
        delta_y.at(i) =
            cut(
                p_distance.at(i) * azimuth.at(i + 1).sin(),
                p_distance_correction_precision);
    }

    double f_x = std::accumulate(delta_x.begin(), delta_x.end(), 0.0) - (p_x_end - p_x_beg);
    double f_y = std::accumulate(delta_y.begin(), delta_y.end(), 0.0) - (p_y_end - p_y_beg);
    double f =
        cut(
            std::sqrt(std::pow(f_x, 2) + std::pow(f_y, 2)), p_distance_correction_precision);
    double Kinv = std::floor(distance_sum / f);

    for (size_t i = 0uz; i != size - 1; ++i)
    {
        correction_delta_x.at(i) =
            cut(
                p_distance.at(i) / distance_sum * -f_x,
                p_distance_correction_precision);
        correction_delta_y.at(i) =
            cut(
                p_distance.at(i) / distance_sum * -f_y,
                p_distance_correction_precision);
    }
    double correction_delta_x_sum =
        std::accumulate(
            correction_delta_x.begin(),
            correction_delta_x.end(),
            0.0);
    double correction_delta_y_sum =
        std::accumulate(
            correction_delta_y.begin(),
            correction_delta_y.end(),
            0.0);
    internal::adjustSumToTarget(
        correction_delta_x,
        correction_delta_x_sum,
        -f_x,
        p_distance_correction_precision);
    internal::adjustSumToTarget(
        correction_delta_y,
        correction_delta_y_sum,
        -f_y,
        p_distance_correction_precision);
    correction_delta_x_sum =
        std::accumulate(
            correction_delta_x.begin(),
            correction_delta_x.end(),
            0.0);
    correction_delta_y_sum =
        std::accumulate(
            correction_delta_y.begin(),
            correction_delta_y.end(),
            0.0);

    for (size_t i = 0uz; i != size - 1; ++i)
    {
        corrected_delta_x.at(i) =
            cut(
                delta_x.at(i) + correction_delta_x.at(i),
                p_distance_correction_precision);
        corrected_delta_y.at(i) =
            cut(
                delta_y.at(i) + correction_delta_y.at(i),
                p_distance_correction_precision);
    }
    double corrected_delta_x_sum =
        std::accumulate(
            corrected_delta_x.begin(),
            corrected_delta_x.end(),
            0.0);
    double corrected_delta_y_sum =
        std::accumulate(
            corrected_delta_y.begin(),
            corrected_delta_y.end(),
            0.0);

    for (size_t i = 0uz; i != size - 1; ++i)
    {
        x.at(i + 1) =
            cut(
                x.at(i) + corrected_delta_x.at(i),
                p_distance_correction_precision);
        y.at(i + 1) =
            cut(
                y.at(i) + corrected_delta_y.at(i),
                p_distance_correction_precision);
    }

    df.load_column("left angle", std::move(p_left_angle));
    df.load_column("angle correction", std::move(angle_correction));
    df.load_column("corrected angle", std::move(corrected_angle));
    df.load_column("azimuth", std::move(azimuth));
    df.load_column("distance", std::move(p_distance));
    df.load_column("delta x", std::move(delta_x));
    df.load_column("delta y", std::move(delta_y));
    df.load_column("correction of delta x", std::move(correction_delta_x));
    df.load_column("correction of delta y", std::move(correction_delta_y));
    df.load_column("corrected delta x", std::move(corrected_delta_x));
    df.load_column("corrected delta y", std::move(corrected_delta_y));
    df.load_column("x", std::move(x));
    df.load_column("y", std::move(y));

    std::vector<std::string>
        angle_sum_name_col{
            "left angle",
            "angle correction",
            "corrected angle",
            "azimuth expect"};
    std::vector<Angle> angle_sum_val_col{
        left_angle_sum,
        Angle::fromSeconds(angle_correction_sum),
        corrected_angle_sum,
        p_angle_end};
    std::vector<std::string> distance_sum_name_col{
        "distance",
        "delta x",
        "delta y",
        "delta x correction",
        "delta y correction",
        "corrected delta x",
        "corrected delta y"};
    std::vector<double> distance_sum_val_col{
        distance_sum,
        f_x,
        f_y,
        correction_delta_x_sum,
        correction_delta_y_sum,
        corrected_delta_x_sum,
        corrected_delta_y_sum};
    std::vector<std::string> aux_info_name_col{
        "f beta",
        "f beta expect",
        "tolerance",
        "f x",
        "f y",
        "f",
        "1/K",
        "x expect",
        "y expect"};
    std::vector<double> aux_info_val_col{
        f_beta,
        f_beta_expect,
        tolerance,
        f_x,
        f_y,
        f,
        Kinv,
        p_x_end,
        p_y_end};
    std::vector<std::string> unit_name_col{
        "angle correction",
        "f beta",
        "f beta expect",
        "tolerance",
        "K",
        "default"};
    std::vector<std::string> unit_val_col{
        "second",
        "second",
        "degree",
        "mm",
        "N/A",
        "m"};

    DataFrame &info = result.info_frame;
    DataFrameColumnIndices info_indices(9);
    std::iota(info_indices.begin(), info_indices.end(), 0);
    info.load_index(std::move(info_indices));
    info.load_column("sum of angle(name)", std::move(angle_sum_name_col));
    info.load_column("sum of angle(val)", std::move(angle_sum_val_col));
    info.load_column("sum of distance(name)", std::move(distance_sum_name_col));
    info.load_column("sum of distance(val)", std::move(distance_sum_val_col));
    info.load_column("aux info(name)", std::move(aux_info_name_col));
    info.load_column("aux info(val)", std::move(aux_info_val_col));
    info.load_column("unit(name)", std::move(unit_name_col));
    info.load_column("unit(val)", std::move(unit_val_col));

    return result;
}

bool closedTraverseAssert(const Adjust_Frame_Result &p_afr)
{
    const DataFrame
        &data = p_afr.frame,
        &info = p_afr.info_frame;
    const auto &x = data.get_column<double>("x");
    const auto &y = data.get_column<double>("y");
    const auto &sum_of_ang_name = info.get_column<std::string>("sum of angle(name)");
    const auto &sum_of_ang_val = info.get_column<Angle>("sum of angle(val)");
    const auto &aux_info_name = info.get_column<std::string>("aux info(name)");
    const auto &aux_info_val = info.get_column<double>("aux info(val)");
    std::size_t sum_of_corrected_angle_idx =
        std::distance(
            sum_of_ang_name.begin(),
            std::find(
                sum_of_ang_name.begin(),
                sum_of_ang_name.end(),
                "corrected angle"));
    std::size_t sum_of_corrected_angle_expect_idx =
        std::distance(
            aux_info_name.begin(),
            std::find(
                aux_info_name.begin(),
                aux_info_name.end(),
                "f beta expect"));

    assert(eqApprox(x.front(), x.back()));
    assert(eqApprox(y.front(), y.back()));
    assert(sum_of_ang_val
               .at(sum_of_corrected_angle_idx)
               .withinAbs(
                   Angle(
                       deg2rad(aux_info_val
                                   .at(sum_of_corrected_angle_expect_idx)))));
    return true;
}

bool connectingTraverseAssert(const Adjust_Frame_Result &p_afr)
{
    const DataFrame
        &data = p_afr.frame,
        &info = p_afr.info_frame;
    const auto &x = data.get_column<double>("x");
    const auto &y = data.get_column<double>("y");
    const auto &sum_of_ang_name = info.get_column<std::string>("sum of angle(name)");
    const auto &sum_of_ang_val = info.get_column<Angle>("sum of angle(val)");
    const auto &aux_info_name = info.get_column<std::string>("aux info(name)");
    const auto &aux_info_val = info.get_column<double>("aux info(val)");
    std::size_t sum_of_corrected_angle_idx =
        std::distance(
            sum_of_ang_name.begin(),
            std::find(
                sum_of_ang_name.begin(),
                sum_of_ang_name.end(),
                "corrected angle"));
    std::size_t sum_of_corrected_angle_expect_idx =
        std::distance(
            aux_info_name.begin(),
            std::find(
                aux_info_name.begin(),
                aux_info_name.end(),
                "f beta expect"));
    std::size_t x_expect_idx =
        std::distance(
            aux_info_name.begin(),
            std::find(
                aux_info_name.begin(),
                aux_info_name.end(),
                "x expect"));
    std::size_t y_expect_idx =
        std::distance(
            aux_info_name.begin(),
            std::find(
                aux_info_name.begin(),
                aux_info_name.end(),
                "y expect"));

    assert(eqApprox(x.at(x.size() - 2), aux_info_val.at(x_expect_idx)));
    assert(eqApprox(y.at(y.size() - 2), aux_info_val.at(y_expect_idx)));
    assert(sum_of_ang_val
               .at(sum_of_corrected_angle_idx)
               .withinAbs(
                   Angle(
                       deg2rad(aux_info_val
                                   .at(sum_of_corrected_angle_expect_idx)))));
    return true;
}

M_libga_end