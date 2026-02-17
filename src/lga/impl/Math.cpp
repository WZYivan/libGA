#include <lga/Math>

M_libga_begin

    bool
    eqApprox(double p_x, double p_y, double p_threshold)
{
    return std::abs(p_x - p_y) < p_threshold;
}

double zeroOr(double p_x, double p_threshold)
{
    return eqApprox(p_x, 0.0, p_threshold) ? 0.0 : p_x;
}

double round465(double p_val)
{
    double int_part;
    double frac_part = std::modf(p_val, &int_part);

    if (eqApprox(frac_part, 0.5))
    {
        if (eqApprox(std::fmod(int_part, 2), 0))
        {
            return int_part;
        }
        else
        {
            return int_part + (p_val >= 0 ? 1 : -1);
        }
    }
    else if (std::abs(frac_part) < 0.5)
    {
        return int_part;
    }
    else // (std::abs(frac_part) > 0.5)
    {
        return int_part + (p_val >= 0 ? 1 : -1);
    }
}

double cut(double p_val, int p_precision)
{
    if (p_precision < 0)
    {
        return p_val;
    }

    if (p_precision == 0)
    {
        return round465(p_val);
    }
    double scale = std::pow(10, p_precision);
    return round465(p_val * scale) / scale;
}

double Sin_Function::operator()(double p_x) const
{
    return a * std::sin(2 * std::numbers::pi / b * p_x + c);
}

double Double_Gaussian_Function::operator()(double p_x) const
{
    return (offset +
            amp1 * std::exp(-0.5 * std::pow((p_x - center1) / sigma1, 2)) +
            amp2 * std::exp(-0.5 * std::pow((p_x - center2) / sigma2, 2)));
}

double invokeCubicSplineFunction(
    const Cubic_Spline_Function &p_fn,
    double p_dx,
    size_t p_idx)
{
    double v{
        p_fn.a[p_idx] +
        p_fn.b[p_idx] * p_dx +
        p_fn.c[p_idx] * std::pow(p_dx, 2) +
        p_fn.d[p_idx] * std::pow(p_dx, 3)};
    return std::abs(v) < p_fn.threshold ? v : p_fn.threshold;
}

double Cubic_Spline_Function::operator()(double p_x) const
{
    if (a.empty())
    {
        return 0.0;
    }

    if (p_x <= x.front())
    {
        double dx = p_x - x[0];
        return invokeCubicSplineFunction(*this, dx, 0);
    }

    if (p_x >= x.back())
    {
        int last_idx = x.size() - 2;
        double dx = p_x - x.back();
        return invokeCubicSplineFunction(*this, dx, last_idx);
    }

    // auto dest =
    //     std::find_if(x.begin(), x.end(),
    //                  [p_x](const auto &val) -> bool
    //                  { return p_x < val; });
    // size_t idx = std::max(std::ptrdiff_t(0), std::distance(x.begin(), dest) - 1);
    size_t idx = 0;
    for (std::size_t i = 0; i != a.size(); ++i)
    {
        if (p_x < x[i + 1])
        {
            idx = i;
            break;
        }
    }

    double dx = p_x - x[idx];
    return invokeCubicSplineFunction(*this, dx, idx);
}

M_libga_end