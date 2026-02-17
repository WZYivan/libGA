#include <lga/Photogrammetry>

M_libga_begin

    Exterior
    Exterior::makeFrom(const Interior &p_in, const Matrix &p_obj)
{
    return Exterior{
        .x = p_obj.col(0).mean(),
        .y = p_obj.col(1).mean(),
        .z = p_obj.col(2).mean() + p_in.m * p_in.f,
        .phi = 0.0,
        .omega = 0.0,
        .kappa = 0.0};
}

Matrix
ex2yxzRotation(const Exterior &p_ex)
{
    return yRotation(p_ex.phi) * xRotation(p_ex.omega) * zRotation(p_ex.kappa);
}

Matrix
xy2mat21(
    double p_x, double p_y)
{
    Matrix m(2, 1);
    m << p_x, p_y;
    return m;
}
Matrix
xyz2mat31(
    double p_x, double p_y, double p_z)
{
    Matrix m(3, 1);
    m << p_x, p_y, p_z;
    return m;
}
Matrix
xy2mat12(
    double p_x, double p_y)
{
    Matrix m(1, 2);
    m << p_x, p_y;
    return m;
}
Matrix
xyz2mat13(
    double p_x, double p_y, double p_z)
{
    Matrix m(1, 3);
    m << p_x, p_y, p_z;
    return m;
}

Matrix
obj2aux(
    const Matrix &p_obj, const Exterior &p_ex)
{
    return translate(p_obj, p_ex.x, p_ex.y, p_ex.z);
}
Matrix
aux2isp(
    const Matrix &p_aux, const Matrix &p_rotate)
{
    return rotateForward(p_aux, p_rotate);
}
Matrix
aux2isp(
    const Matrix &p_aux,
    const Exterior &p_ex,
    std::function<Matrix(const Exterior &)> p_calc_rotate)
{
    return aux2isp(p_aux, p_calc_rotate(p_ex));
}
Matrix
isp2img(
    const Matrix &p_isp, const Interior &p_in)
{
    Matrix img(p_isp.rows(), 2);
    const auto &XBar = p_isp.col(0).array(),
               &YBar = p_isp.col(1).array(),
               &ZBar = p_isp.col(2).array();
    const double f = p_in.f;
    img.col(0) = -f * (XBar / ZBar).matrix(); // x
    img.col(1) = -f * (YBar / ZBar).matrix(); // y
    return img;
}

Matrix
obj2img(
    const Matrix &p_obj,
    const Exterior &p_ex,
    const Interior &p_in, const Matrix &p_rotate)
{
    return isp2img(aux2isp(obj2aux(p_obj, p_ex), p_rotate), p_in);
}
Matrix
obj2img(
    const Matrix &p_obj, const Exterior &p_ex,
    const Interior &p_in,
    std::function<Matrix(const Exterior &)> p_calc_rotate)
{
    return obj2img(p_obj, p_ex, p_in, p_calc_rotate(p_ex));
}

Matrix
isp2aux(
    const Matrix &p_isp, const Matrix &p_rotate)
{
    return rotateInverse(p_isp, p_rotate);
}
Matrix
isp2aux(
    const Matrix &p_isp, const Exterior &p_ex, std::function<Matrix(const Exterior &)> p_calc_rotate)
{
    return isp2aux(p_isp, p_calc_rotate(p_ex));
}

Matrix
aux2obj(
    const Matrix &p_aux, const Exterior &p_ex)
{
    return translate(p_aux, p_ex.x, p_ex.y, p_ex.z);
}

Matrix
Collinearity_Condition_Equation_Coefficient::toMatrix26() const noexcept
{
    Matrix mat(2, 6);
    mat << a11, a12, a13, a14, a15, a16,
        a21, a22, a23, a24, a25, a26;
    return mat;
}

Matrix
Collinearity_Condition_Equation_Coefficient::toMatrix29() const noexcept
{
    Matrix mat(2, 9);
    mat << a11, a12, a13, a14, a15, a16, -a11, -a12, -a13,
        a21, a22, a23, a24, a25, a26, -a21, -a22, -a23;
    return mat;
}

Collinearity_Condition_Equation_Coefficient
cceSimplifyAll(
    Collinearity_Condition_Equation_Linearization_Param p)
{
    double
        x = p.x,
        y = p.y,
        f = p.f, H = p.h;

    double
        xx = f + pow(x, 2) / f,
        yy = f + pow(y, 2) / f,
        xy = x * y / f;

    Collinearity_Condition_Equation_Coefficient
        coeff{
            .a11 = -f / H,
            .a12 = 0,
            .a13 = -x / H,
            .a14 = -xx,
            .a15 = -xy,
            .a16 = y,
            .a21 = 0,
            .a22 = -f / H,
            .a23 = -y / H,
            .a24 = -xy,
            .a25 = -yy,
            .a26 = -x};
    return coeff;
}
Collinearity_Condition_Equation_Coefficient
cceKappaOnly(
    Collinearity_Condition_Equation_Linearization_Param p)
{
    double
        x = p.x,
        y = p.y,
        f = p.f, H = p.h,
        k = p.kappa;

    double
        xx = f + pow(x, 2) / f,
        yy = f + pow(y, 2) / f,
        xy = x * y / f,
        cosk = std::cos(k),
        sink = std::sin(k);

    Collinearity_Condition_Equation_Coefficient
        coeff{
            .a11 = -f / H * cosk,
            .a12 = -f / H * sink,
            .a13 = -x / H,
            .a14 = -xx * cosk + xy * sink,
            .a15 = -xy * cosk - xx * sink,
            .a16 = y,
            .a21 = f / H * sink,
            .a22 = -f / H * cosk,
            .a23 = -y / H,
            .a24 = -xy * cosk + yy * sink,
            .a25 = -yy * cosk - xy * sink,
            .a26 = -x};
    return coeff;
}
Collinearity_Condition_Equation_Coefficient
cceSimplifyNone(
    Collinearity_Condition_Equation_Linearization_Param p)
{
    double
        x = p.x,
        y = p.y, z = p.z,
        f = p.f, H = p.h,
        k = p.kappa, w = p.omega;
    const Matrix &rotate = p.rotate;

    double
        cosk = std::cos(k),
        sink = std::sin(k),
        cosw = std::cos(w),
        sinw = std::sin(w);
    const auto &a = rotate.row(0),
               &b = rotate.row(1),
               &c = rotate.row(2);

    Collinearity_Condition_Equation_Coefficient
        coeff{
            .a11 = 1 / z * (a(0) * f + a(2) * x),
            .a12 = 1 / z * (b(0) * f + b(2) * x),
            .a13 = 1 / z * (c(0) * f + c(2) * x),
            .a14 = y * sinw - (x / f * (x * cosk - y * sink) + f * cosk) * cosw,
            .a15 = -f * sink - x / f * (x * sink + y * cosk),
            .a16 = y,
            .a21 = 1 / z * (a(1) * f + a(2) * y),
            .a22 = 1 / z * (b(1) * f + b(2) * y),
            .a23 = 1 / z * (c(1) * f + c(2) * y),
            .a24 = -x * sinw - (y / f * (x * cosk - y * sink) - f * sink) * cosw,
            .a25 = -f * cosk - y / f * (x * sink + y * cosk),
            .a26 = -x};
    return coeff;
}

Space_Resection_Result
spaceResection(
    const Interior &p_in,
    const Matrix &p_img,
    const Matrix &p_obj,
    size_t p_max_loop,
    double p_threshold,
    Collinearity_Condition_Equation_Coefficient_Solver p_cce_solver,
    Matrix_Inverse_Solver p_inverse_solver)
{
    if (!(
            p_img.rows() == p_obj.rows() &&
            p_img.cols() == 2 &&
            p_obj.cols() == 3))
    {
        throw std::invalid_argument(
            std::format(
                "matrix shape does't satisfy rules: {:d} == {:d} && {:d} == 2 && {:d} == 3",
                p_img.rows(),
                p_obj.rows(),
                p_img.cols(),
                p_obj.cols()));
    }

    if (!(p_img.rows() >= 4))
    {
        throw std::invalid_argument(
            std::format(
                "`{:d}` points are not enough for space resection",
                p_img.rows()));
    }

    Space_Resection_Result result{
        .exterior{Exterior::makeFrom(p_in, p_obj)},
        .info{Iterative_Algo_Info::Not_Converged}};

    Exterior &exterior = result.exterior;

    while (p_max_loop-- > 0)
    {
        Matrix
            rotate{ex2yxzRotation(exterior)},
            isp{aux2isp(obj2aux(p_obj, exterior), rotate)},
            img_calc{isp2img(isp, p_in)},
            dxy = p_img - img_calc;

        Matrix residual(p_img.rows() * 2, 1);
        for (auto li = 0uz; li != dxy.rows(); ++li)
        {
            residual(2 * li) = dxy(li, 0);
            residual(2 * li + 1) = dxy(li, 1);
        }

        Matrix coefficient(p_img.rows() * 2, 6);
        Collinearity_Condition_Equation_Linearization_Param
            cce_param{
                .f = p_in.f,
                .h = p_in.f * p_in.m,
                .kappa = exterior.kappa,
                .omega = exterior.omega,
                .rotate = rotate};
        for (auto pi = 0uz; pi != img_calc.rows(); ++pi)
        {
            cce_param.x = img_calc(pi, 0);
            cce_param.y = img_calc(pi, 1);
            cce_param.z = isp(pi, 2);

            Collinearity_Condition_Equation_Coefficient
                c = p_cce_solver(cce_param);
            coefficient.row(pi * 2) << c.a11, c.a12, c.a13, c.a14, c.a15, c.a16;
            coefficient.row(pi * 2 + 1) << c.a21, c.a22, c.a23, c.a24, c.a25, c.a26;
        }

        Matrix correction{ols(coefficient, residual)};

        if (std::abs(correction(3)) < p_threshold &&
            std::abs(correction(4)) < p_threshold &&
            std::abs(correction(5)) < p_threshold)
        {
            Matrix v = coefficient * correction - residual;
            Matrix n_inv = p_inverse_solver(coefficient.transpose() * coefficient);
            result.rmse = rmse(v, coefficient.rows(), 6);
            result.sigma = errorMatrix(result.rmse, n_inv);
            result.image = Matrix(p_img);
            result.rotate = Matrix(rotate);
            Matrix &p = result.image;
            for (size_t pc = coefficient.rows() / 2, pi = 0uz; pi != pc; ++pi)
            {
                p(pi, 0) = (p(pi, 0) + v(2 * pi)) * 1000;
                p(pi, 1) = (p(pi, 1) + v(2 * pi + 1)) * 1000;
            }
            result.info = Iterative_Algo_Info::Success;
            break;
        }

#if (M_libga_debug)
        Eigen::IOFormat fmt(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");
        std::cout << "\nrotate\n"
                  << rotate.format(fmt)
                  << "\nobj\n"
                  << p_obj.format(fmt)
                  << "\nisp\n"
                  << isp.format(fmt)
                  << "\nimg_calc\n"
                  << img_calc.format(fmt)
                  << "\nresidual\n"
                  << residual.format(fmt)
                  << "\ncoefficient\n"
                  << coefficient.format(fmt)
                  << "\ncorrection\n"
                  << correction.format(fmt)
                  << std::endl;

        std::println(
            R"(
exterior :
    x = {:.5f}
    y = {:.5f}
    z = {:.5f}
    phi = {:.5f}
    omega = {:.5f}
    kappa = {:.5f}
    )",
            exterior.x,
            exterior.y,
            exterior.z,
            exterior.phi,
            exterior.omega,
            exterior.kappa);
#endif

        exterior.x += correction(0);
        exterior.y += correction(1);
        exterior.z += correction(2);
        exterior.phi += correction(3);
        exterior.omega += correction(4);
        exterior.kappa += correction(5);
    }

    return result;
}

Matrix
spaceIntersection(
    const Image_Meta_Data &p_left_meta,
    const Matrix &p_left_img,
    const Image_Meta_Data &p_right_meta,
    const Matrix &p_right_img)
{
    Matrix
        isp_left = p_left_img,
        isp_right = p_right_img;

    isp_left.conservativeResize(Eigen::NoChange, 3);
    isp_left.col(2).fill(-p_left_meta.interior.f);
    isp_right.conservativeResize(Eigen::NoChange, 3);
    isp_right.col(2).fill(-p_right_meta.interior.f);

    Matrix
        aux_left = isp2aux(isp_left, p_left_meta.exterior),
        aux_right = isp2aux(isp_right, p_right_meta.exterior);
    double Xs1 = p_left_meta.exterior.x, Xs2 = p_right_meta.exterior.x,
           Ys1 = p_left_meta.exterior.y, Ys2 = p_right_meta.exterior.y,
           Zs1 = p_left_meta.exterior.z, Zs2 = p_right_meta.exterior.z;
    double BX = Xs2 - Xs1,
           BY = Ys2 - Ys1,
           BZ = Zs2 - Zs1;
    auto
        X1 = aux_left.col(0).array(),
        Y1 = aux_left.col(1).array(),
        Z1 = aux_left.col(2).array(),
        X2 = aux_right.col(0).array(),
        Y2 = aux_right.col(1).array(),
        Z2 = aux_right.col(2).array();
    auto
        N1 = (BX * Z2 - BZ * X2) / (X1 * Z2 - Z1 * X2),
        N2 = (BX * Z1 - BZ * X1) / (X1 * Z2 - Z1 * X2);
    auto X = Xs1 + BX + N2 * X2;
    auto Y = Ys1 + (N1 * Y1 + N2 * Y2 + BY) / 2.0;
    auto Z = Zs1 + BZ + N2 * Z2;

    Matrix result(X.rows(), 3);
    result.col(0) << X.matrix();
    result.col(1) << Y.matrix();
    result.col(2) << Z.matrix();
    return result;
}

Space_Intersection_Ols_Result
spaceIntersection(
    std::span<const Space_Intersection_Ols_Block> p_list,
    size_t p_max_loop,
    double p_threshold,
    Collinearity_Condition_Equation_Coefficient_Solver p_cce_solver,
    Matrix_Inverse_Solver p_inverse_solver)
{
    size_t count = p_list.size();
    if (count < 2)
    {
        throw std::invalid_argument("input param less than 2");
    }

    Space_Intersection_Ols_Result result{
        .coordinate = spaceIntersection(
            p_list[0].meta, p_list[0].image,
            p_list[1].meta, p_list[1].image),
        .info = Iterative_Algo_Info::Not_Converged};
    Matrix &coordinate = result.coordinate;

#if (M_libga_debug)
    std::cout << "> coordinate init\n"
              << coordinate.format(fmt::python)
              << "\n";
#endif

    while (p_max_loop-- > 0)
    {
        bool is_converged = true;

        for (size_t row = 0uz; row != coordinate.rows(); ++row)
        {

            Matrix coeff(2 * count, 3), residual(2 * count, 1);
            for (size_t i = 0uz; i != count; ++i)
            {
                const Image_Meta_Data &meta = p_list[i].meta;
                const Matrix &img = p_list[i].image.row(i);
                const Exterior &ex = meta.exterior;
                const Interior &in = meta.interior;

                const Matrix
                    rotate = ex2yxzRotation(ex),
                    &obj = coordinate.row(i),
                    isp = aux2isp(obj2aux(obj, ex), rotate),
                    img_calc = isp2img(isp, in);
#if (M_libga_debug)
                std::cout
                    << "> obj\n"
                    << obj.format(fmt::python)
                    << "\n"
                    << "> img_calc\n"
                    << img_calc.format(fmt::python)
                    << "\n"
                    << "> img\n"
                    << img.format(fmt::python)
                    << "\n";
#endif
                Matrix dxy = img - img_calc;
                Collinearity_Condition_Equation_Coefficient c =
                    p_cce_solver({.x = img_calc(0, 0),
                                  .y = img_calc(0, 1),
                                  .f = in.f,
                                  .h = in.f * in.m,
                                  .kappa = ex.kappa,
                                  .omega = ex.omega,
                                  .z = isp(0, 2),
                                  .rotate = rotate

                    });

                Matrix l(dxy.rows() * 2, 1);
                for (auto li = 0uz; li != dxy.rows(); ++li)
                {
                    l(2 * li) = dxy(li, 0);
                    l(2 * li + 1) = dxy(li, 1);
                }

#if (M_libga_debug)
                std::cout
                    << "> l\n"
                    << l.format(fmt::python)
                    << "\n"
                    << "> dxy\n"
                    << dxy.format(fmt::python)
                    << "\n";
#endif

                coeff.block(2 * i, 0, 2, 3)
                    << -c.toMatrix26().leftCols(3);
                residual.block(2 * i, 0, 2, 1) << l;
            }

            const Matrix correction = ols(coeff, residual);

#if (M_libga_debug)
            std::cout
                << "residual\n"
                << residual.format(fmt::python)
                << "\n"
                << "coeff\n"
                << coeff.format(fmt::python)
                << "\n"
                << "correction\n"
                << correction.format(fmt::python)
                << "\n";
#endif

            if (std::abs(correction(0, 0)) < p_threshold &&
                std::abs(correction(1, 0)) < p_threshold &&
                std::abs(correction(2, 0)) < p_threshold)
            {
                result.info = Iterative_Algo_Info::Success;
                const Matrix N = p_inverse_solver(coeff.transpose() * coeff);
                result.rmse = rmse(coeff * correction - residual, count * 2, 3);
                result.sigma = errorMatrix(result.rmse, N);
            }
            else
            {
                is_converged = false;
            }

            coordinate(row, 0) += correction(0, 0);
            coordinate(row, 1) += correction(1, 0);
            coordinate(row, 2) += correction(2, 0);
        }
        if (is_converged)
        {
            break;
        }
    }
    return result;
}

M_libga_end