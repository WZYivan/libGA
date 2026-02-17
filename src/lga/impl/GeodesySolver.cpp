#include <lga/Geodesy>

M_libga_begin

    Geodetic_Forward_Solve_Result
    Geodetic_Solver::operator()(
        const Latitude &p_lat,
        const Longitude &p_lon,
        double p_s,
        const Angle &p_forward,
        const Ellipsoid &p_ellipsoid) const
{
    return this->forward(
        p_lat,
        p_lon,
        p_s,
        p_forward,
        p_ellipsoid);
}

Geodetic_Inverse_Solve_Result
Geodetic_Solver::operator()(
    const Latitude &p_lat_1,
    const Longitude &p_lon_1,
    const Latitude &p_lat_2,
    const Longitude &p_lon_2,
    const Ellipsoid &p_ellipsoid) const
{
    return this->inverse(
        p_lat_1,
        p_lon_1,
        p_lat_2,
        p_lon_2,
        p_ellipsoid);
}

struct Bessel_Formula_Coeff
{
    double A, B, C, alpha, beta, beta_prime;
};

struct Gauss_Lemma_Coeff
{
    double r01, r21, r03, S10, S12, S30, t01, t21, t03;

    inline double u(double dLs, double dBs) const noexcept
    {
        return r01 * dLs + r21 * std::pow(dBs, 2) * dLs + r03 * std::pow(dLs, 3);
    }

    inline double v(double dLs, double dBs) const noexcept
    {
        return S10 * dBs + S12 * dBs * std::pow(dLs, 2) + S30 * std::pow(dBs, 3);
    }

    inline double deltaAs(double dLs, double dBs) const noexcept
    {
        return t01 * dLs + t21 * std::pow(dBs, 2) * dLs + t03 * std::pow(dLs, 3);
    }
};

namespace internal
{
    Bessel_Formula_Coeff
    calcBesselFormulaCoeff(
        double cosA0p2,
        const Ellipsoid &p_ellipsoid)
    {
        Bessel_Formula_Coeff coeff;

        double
            e2 = p_ellipsoid.geometry.e1_2,
            e2_2 = p_ellipsoid.geometry.e2_2,
            b = p_ellipsoid.geometry.b,
            k2 = e2_2 * cosA0p2,
            k4 = std::pow(k2, 2),
            k6 = std::pow(k2, 3),
            e4 = std::pow(e2, 2),
            e6 = std::pow(e2, 3),
            cosA0p4 = std::pow(cosA0p2, 2);

        coeff.A = b * (1 +
                       k2 / 4.0 -
                       3 * k4 / 64.0 +
                       5 * k6 / 256.0);
        coeff.B = b * (k2 / 8.0 -
                       k4 / 32.0 +
                       15 * k6 / 1024.0);
        coeff.C = b * (k4 / 128.0 - 3 * k6 / 512.0);
        coeff.alpha = (e2 / 2.0 + e4 / 8.0 + e6 / 16.0) -
                      (e4 / 16.0 + e6 / 16.0) * cosA0p2 +
                      (3 * e6 / 128.0) * cosA0p4;
        coeff.beta = (e4 / 32.0 + e6 / 32.0) * cosA0p2 -
                     (e6 / 64.0) * cosA0p4;
        coeff.beta_prime = 2 * coeff.beta / cosA0p2;

        return coeff;
    }

    double adjustLambda(double lambda, double sinA1, double tan_lambda)
    {
        double abs_lambda = std::abs(lambda);

        if (sinA1 > 0 && tan_lambda > 0)
        {
            return abs_lambda;
        }
        else if (sinA1 > 0 && tan_lambda < 0)
        {
            return deg2rad(180) - abs_lambda;
        }
        else if (sinA1 < 0 && tan_lambda < 0)
        {
            return -abs_lambda;
        }
        else if (sinA1 < 0 && tan_lambda > 0)
        {
            return abs_lambda - deg2rad(180);
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    double adjustA2(double A2, double sinA1, double tanA2)
    {
        double absA2 = std::abs(A2);

        if (sinA1 < 0 && tanA2 > 0)
        {
            return absA2;
        }
        else if (sinA1 < 0 && tanA2 < 0)
        {
            return deg2rad(180) - absA2;
        }
        else if (sinA1 > 0 && tanA2 > 0)
        {
            return deg2rad(180) + absA2;
        }
        else if (sinA1 > 0 && tanA2 < 0)
        {
            return deg2rad(360) - absA2;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    double adjustA1(double A1, double p, double q)
    {
        double absA1 = std::abs(A1);

        if (p > 0 && q > 0)
        {
            return absA1;
        }
        else if (p > 0 && q < 0)
        {
            return deg2rad(180) - absA1;
        }
        else if (p < 0 && q < 0)
        {
            return deg2rad(180) + absA1;
        }
        else if (p < 0 && q > 0)
        {
            return deg2rad(360) - absA1;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    double adjustSigma(double sigma, double cos_sigma)
    {
        double abs_sigma = std::abs(sigma);

        if (cos_sigma > 0)
        {
            return abs_sigma;
        }
        else if (cos_sigma < 0)
        {
            return deg2rad(180) - abs_sigma;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    Gauss_Lemma_Coeff
    calcGaussLemmaCoeff(
        const Latitude &Bm,
        const Ellipsoid &ellipsoid)
    {
        Gauss_Lemma_Coeff coeff;
        Ellipsoid_Geometry_Latitude_Aux lat_const(
            Bm, ellipsoid);
        double
            V = lat_const.v,
            t = lat_const.t,
            n2 = lat_const.nu_2,
            cosB = Bm.cos(),
            ps = rho2;
        Ellipsoid_Principle_Curvature_Radius crc =
            principleCurvatureRadius(Bm, ellipsoid);
        double N = crc.n;

        double
            p2 = std::pow(ps, 2),
            p3 = std::pow(ps, 3),
            V2 = std::pow(V, 2),
            V4 = std::pow(V, 4),
            V6 = std::pow(V, 6),
            n4 = std::pow(n2, 2),
            t2 = std::pow(t, 2),
            cosB2 = std::pow(cosB, 2),
            cosB3 = std::pow(cosB, 3);

        coeff.r01 = N / ps * cosB;
        coeff.r21 = (N * cosB) / (24 * p3 * V4) * (1 + n2 - 9 * n2 * t2 + n4);
        coeff.r03 = -N / (24 * p3) * (cosB3 * t2);
        coeff.S10 = N / (ps * V2);
        coeff.S12 = N / (24 * p3 * V2) * cosB2 * (2 + 3 * t2 + 2 * n2);
        coeff.S30 = N / (8 * p3 * V6) * (n2 - t2 * n2);
        coeff.t01 = t * cosB;
        coeff.t21 = 1.0 / (24 * p2 * V4) * cosB * t * (2 + 7 * n2 + 9 * t2 * n2 + 5 * n4);
        coeff.t03 = 1.0 / (24 * p2) * cosB3 * t * (2 + t2 + 2 * n2);

        return coeff;
    }

    double calcAmRad(double T, double dBs, double dLs)
    {
        if (dBs > 0 && dLs >= 0)
        {
            return T;
        }
        else if (dBs < 0 && dLs >= 0)
        {
            return std::numbers::pi - T;
        }
        else if (dBs <= 0 && dLs < 0)
        {
            return std::numbers::pi + T;
        }
        else if (dBs > 0 && dLs < 0)
        {
            return 2.0 * std::numbers::pi - T;
        }
        else if (dBs == 0 && dLs >= 0)
        {
            return std::numbers::pi / 2.0;
        }
        else
        {
            return std::numeric_limits<double>::signaling_NaN();
        }
    }

    void
    initGaussLemmaForwardSolveIteration(
        const Longitude &L,
        const Latitude &B,
        double S,
        const Angle &a_backward,
        double &b,
        double &l,
        double &a,
        const Ellipsoid &ellipsoid)
    {
        double p = rho2;
        Ellipsoid_Principle_Curvature_Radius crc =
            principleCurvatureRadius(B, ellipsoid);
        double M = crc.m, N = crc.n;

        b = p / M * a_backward.cos();
        l = p / N * a_backward.sin() * (1.0 / B.cos());
        a = l * B.sin();
    }
}

Geodetic_Forward_Solve_Result
besselFormulaForward(
    const Latitude &p_B1,
    const Longitude &p_L1,
    double p_S,
    const Angle &p_forward,
    const Ellipsoid &p_ellipsoid)
{
    Ellipsoid_Geometry_Latitude_Aux lc(p_B1, p_ellipsoid);
    double
        W1 = lc.w,
        e2 = p_ellipsoid.geometry.e1_2,
        sinB1 = p_B1.sin(),
        cosB1 = p_B1.cos(),
        sinu1 = sinB1 * std::sqrt(1 - e2) / W1,
        cosu1 = cosB1 / W1,
        sinA1 = p_forward.sin(),
        cosA1 = p_forward.cos(),
        sinA0 = cosu1 * sinA1,
        sinA0p2 = std::pow(sinA0, 2),
        cosA0p2 = 1 - sinA0p2,
        cot_sigma1 = cosu1 * cosA1 / sinu1,
        cot_sigma1p2 = std::pow(cot_sigma1, 2),
        sin_2sigma1 = 2.0 * cot_sigma1 / (cot_sigma1p2 + 1),
        cos_2sigma1 = (cot_sigma1p2 - 1) / (cot_sigma1p2 + 1);

    Bessel_Formula_Coeff coeff_solver{
        internal::calcBesselFormulaCoeff(
            cosA0p2,
            p_ellipsoid)};
    double
        A = coeff_solver.A,
        B = coeff_solver.B,
        C = coeff_solver.C,
        alpha = coeff_solver.alpha,
        beta = coeff_solver.beta;

    double
        sigma0 = (p_S - (B + C * cos_2sigma1) * sin_2sigma1) / A,
        sin_2sigma0 = std::sin(2 * sigma0),
        cos_2sigma0 = std::cos(2 * sigma0),
        sin_2_sigma1_a_sigma0 = sin_2sigma1 * cos_2sigma0 + cos_2sigma1 * sin_2sigma0,
        cos_2_sigma1_a_sigma0 = cos_2sigma1 * cos_2sigma0 - sin_2sigma1 * sin_2sigma0,
        sigma = sigma0 + (B + 5 * C * cos_2_sigma1_a_sigma0) * sin_2_sigma1_a_sigma0 / A,
        sin_sigma = std::sin(sigma),
        cos_sigma = std::cos(sigma),
        delta = (alpha * sigma + beta * (sin_2_sigma1_a_sigma0 - sin_2sigma1)) * sinA0,
        sinu2 = sinu1 * cos_sigma + cosu1 * cosA1 * sin_sigma,
        sinu2p2 = std::pow(sinu2, 2),
        B2 = std::atan(
            sinu2 / (std::sqrt(1 - e2) * std::sqrt(1 - sinu2p2))),
        lambda = std::atan(
            (sinA1 * sin_sigma) / (cosu1 * cos_sigma - sinu1 * sin_sigma * cosA1)),
        tan_lambda = std::tan(lambda);
    lambda = internal::adjustLambda(lambda, sinA1, tan_lambda);
    double
        L2 = p_L1.rad() + lambda - delta,
        A2 = std::atan(
            cosu1 * sinA1 / (cosu1 * cos_sigma * cosA1 - sinu1 * sin_sigma)),
        tanA2 = std::tan(A2);
    A2 = internal::adjustA2(A2, sinA1, tanA2);

    return {
        .lat = Latitude(B2),
        .lon = Longitude(L2),
        .backward = Angle(A2)};
}

Geodetic_Inverse_Solve_Result
besselFormulaInverse(
    const Latitude &B1,
    const Longitude &L1,
    const Latitude &B2,
    const Longitude &L2,
    const Ellipsoid &ellipsoid)
{
    Ellipsoid_Geometry_Latitude_Aux
        lc1(B1, ellipsoid),
        lc2(B2, ellipsoid);

    double
        W1 = lc1.w,
        W2 = lc2.w,
        sinB1 = B1.sin(),
        cosB1 = B1.cos(),
        sinB2 = B2.sin(),
        cosB2 = B2.cos(),
        e2 = ellipsoid.geometry.e1_2,
        sqrt_1_s_e2 = std::sqrt(1 - e2),
        sinu1 = sinB1 * sqrt_1_s_e2 / W1,
        sinu2 = sinB2 * sqrt_1_s_e2 / W2,
        cosu1 = cosB1 / W1,
        cosu2 = cosB2 / W2,
        L = L2.rad() - L1.rad(),
        a1 = sinu1 * sinu2,
        a2 = cosu1 * cosu2,
        b1 = cosu1 * sinu2,
        b2 = sinu1 * cosu2;

    double
        delta = 0,
        lambda = L + delta,
        A1,
        sigma,
        x,
        sinA0,
        delta_p,
        lambda_p;

    do
    {
        delta_p = delta;
        lambda_p = lambda;
        double
            cos_lambda = std::cos(lambda),
            sin_lambda = std::sin(lambda);

        double
            p = cosu2 * sin_lambda,
            q = b1 - b2 * cos_lambda;
        A1 = std::atan(p / q);
        A1 = internal::adjustA1(A1, p, q);

        double
            sinA1 = std::sin(A1),
            cosA1 = std::cos(A1),
            sin_sigma = p * sinA1 + q * cosA1,
            cos_sigma = a1 + a2 * cos_lambda;
        sigma = std::atan(sin_sigma / cos_sigma);
        sigma = internal::adjustSigma(sigma, cos_sigma);

        sinA0 = cosu1 * sinA1;
        double
            sinA0p2 = std::pow(sinA0, 2),
            cosA0p2 = 1 - sinA0p2;
        x = 2 * a1 - cosA0p2 * cos_sigma;

        Bessel_Formula_Coeff coeff_solver = internal::calcBesselFormulaCoeff(
            cosA0p2,
            ellipsoid);
        double
            alpha = coeff_solver.alpha,
            beta_prime = coeff_solver.beta_prime;

        delta = (alpha * sigma - beta_prime * x * sin_sigma) * sinA0;
        lambda = L + delta;

    } while (
        !(
            std::abs(delta - delta_p) < config::geodesy.bessel_formula_solve.threshold &&
            std::abs(lambda - lambda_p) < config::geodesy.bessel_formula_solve.threshold));

    double
        sinA0p2 = std::pow(sinA0, 2),
        cosA0p2 = 1 - sinA0p2,
        cosA0p4 = std::pow(cosA0p2, 2),
        x2 = std::pow(x, 2),
        cos_sigma = std::cos(sigma),
        sin_sigma = std::sin(sigma),
        sin_lambda = std::sin(lambda),
        cos_lambda = std::cos(lambda),
        y = (cosA0p4 - 2 * x2) * cos_sigma;

    Bessel_Formula_Coeff coeff_solver = internal ::calcBesselFormulaCoeff(cosA0p2, ellipsoid);
    double
        A = coeff_solver.A,
        B_pp = 2 * coeff_solver.B / cosA0p2,
        C_pp = 2 * coeff_solver.C / cosA0p4,
        S = A * sigma + (B_pp * x + C_pp * y) * sin_sigma,
        A2 = std::atan(
            cosu1 * sin_lambda / (b1 * cos_lambda - b2));
    double d180r = deg2rad(180);
    A2 = (A1 < d180r ? 1 : -1) * d180r + A2;

    return {
        .forward = Angle(A1),
        .backward = Angle(A2),
        .s = S};
}

Geodetic_Forward_Solve_Result
gaussLemmaForward(
    const Latitude &B,
    const Longitude &L,
    double S,
    const Angle &a_forward,
    const Ellipsoid &ellipsoid)
{
    double dB0, dL0, dA0;
    internal::initGaussLemmaForwardSolveIteration(L, B, S, a_forward, dB0, dL0, dA0, ellipsoid);
    double
        Bm = rad2sec(B.rad()) + dB0 / 2.0,
        Am = rad2sec(a_forward.toRadian()) + dA0 / 2.0;

    double dB{}, dL{}, dA{}, dBp{}, dLp{}, dAp{};

    do
    {
        dBp = dB;
        dLp = dL;
        dAp = dA;

        Ellipsoid_Geometry_Latitude_Aux lc(Latitude(sec2rad(Bm)), ellipsoid);
        Ellipsoid_Principle_Curvature_Radius crc = principleCurvatureRadius(Latitude(sec2rad(Bm)), ellipsoid);
        double
            N = crc.n,
            N2 = std::pow(N, 2),
            V = lc.v,
            V2 = std::pow(V, 2),
            S2 = std::pow(S, 2),
            t = lc.t,
            t2 = std::pow(t, 2),
            n2 = lc.nu_2,
            n4 = std::pow(n2, 2),
            p = rho2,
            cosA = std::cos(sec2rad(Am)),
            cosA2 = std::pow(cosA, 2),
            sinA = std::sin(sec2rad(Am)),
            sinA2 = std::pow(sinA, 2),
            secB = 1.0 / std::cos(sec2rad(Bm));

        dB = V2 / N * p * S * cosA * (1 + S2 / (24 * N2) * (sinA2 * (2 + 3 * t2 + 3 * n2 * t2) + 3 * n2 * cosA2 * (-1 + t2 - n2 - 4 * t2 * n2)));
        dL = p / N * S * secB * sinA * (1 + S2 / (24 * N2) * (sinA2 * t2 - cosA2 * (1 + n2 - 9 * t2 * n2 + n4)));
        dA = p / N * S * sinA * t * (1 + S2 / (24 * N2) * (cosA2 * (2 + 7 * n2 + 9 * t2 * n2 + 5 * n4) + sinA2 * (2 + t2 + 2 * n2)));

        Bm = rad2sec(B.rad()) + dB / 2.0;
        Am = rad2sec(a_forward.toRadian()) + dA / 2.0;
    } while (
        !((std::abs(dB - dBp) < config::geodesy.gauss_lemma_solve.threshold) &&
          (std::abs(dL - dLp) < config::geodesy.gauss_lemma_solve.threshold) &&
          (std::abs(dA - dAp) < config::geodesy.gauss_lemma_solve.threshold)));

    Longitude L_tar(L.rad() + sec2rad(dL));
    Latitude B_tar(B.rad() + sec2rad(dB));
    double d180s = 180 * 3600, d180r = deg2rad(180);
    Angle a_backward(a_forward.toRadian() + sec2rad(dA + (a_forward.toRadian() < d180r ? 1 : -1) * d180s)); // <-- units
    return Geodetic_Forward_Solve_Result{
        .lat = B_tar,
        .lon = L_tar,
        .backward = a_backward};
}

Geodetic_Inverse_Solve_Result
gaussLemmaInverse(
    const Latitude &B1,
    const Longitude &L1,
    const Latitude &B2,
    const Longitude &L2,
    const Ellipsoid &ellipsoid)
{
    double
        dLs = rad2sec(L2.rad() - L1.rad()),
        dBs = rad2sec(B2.rad() - B1.rad());
    Latitude Bm((B1.rad() + B2.rad()) / 2.0);

    Gauss_Lemma_Coeff coeff_solver = internal::calcGaussLemmaCoeff(Bm, ellipsoid);

    double
        U = coeff_solver.u(dLs, dBs),
        V = coeff_solver.v(dLs, dBs),
        dAs = coeff_solver.deltaAs(dLs, dBs),
        tanA [[maybe_unused]] = U / V;

    double
        c = std::abs(V / U),
        T = (std::abs(dBs) >= std::abs(dLs)) ? std::atan(U / V) : std::numbers::pi / 4.0 + std::atan((1.0 - c) / (1.0 + c)),
        Am_rad = internal::calcAmRad(T, dBs, dLs),
        Am = rad2sec(Am_rad);

    double
        S = (U / std::sin(Am_rad) + V / std::cos(Am_rad)) / 2.0,
        A12 = Am - dAs / 2.0,
        d180s = 180 * 3600,
        A21 = Am + dAs / 2.0 + (A12 < d180s ? 1 : -1) * d180s;
    return Geodetic_Inverse_Solve_Result{
        .forward = Angle(sec2rad(A12)),
        .backward = Angle(sec2rad(A21)),
        .s{S}};
}

const Geodetic_Solver
    gauss_lemma_solve{
        .forward{gaussLemmaForward},
        .inverse{gaussLemmaInverse}},
    bessel_formula_solve{
        .forward{besselFormulaForward},
        .inverse{besselFormulaInverse}};

M_libga_end