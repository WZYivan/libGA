#include <lga/Linalg>

M_libga_begin

    void
    fillNan(Matrix &p_mat, double p_val)
{
    p_mat = p_mat.array().isNaN().select(p_val, p_mat.array());
}

bool isPlaceHolder(const Matrix &p)
{
    return p.isZero() && p.rows() == 1 && p.cols() == 1;
}

bool isValidCoordinate(const Matrix &p_coord)
{
    return p_coord.cols() == 3;
}

bool isValidRotationMatrix(const Matrix &p_coord)
{
    return p_coord.cols() == 3 && p_coord.rows() == 3;
}

void matrixBadShape(int p_r, int p_c, int p_r_e, int p_c_e)
{
    throw Matrix_Shape_Error("Bad matrix shape", p_r, p_c, p_r_e, p_c_e);
}

Matrix normalizedEquation(const Matrix &p_A, const Matrix &p_P)
{
    if (isPlaceHolder(p_P) || p_P.isIdentity())
    {
        return p_A.transpose() * p_A;
    }
    else
    {
        return p_A.transpose() * p_P * p_A;
    }
}

Matrix identityLike(const Matrix &p)
{
    return Matrix::Identity(p.rows(), p.cols());
}

Matrix choleskyInverse(const Matrix &p)
{
    Eigen::LLT<Matrix> llt(p);
    if (llt.info() == Eigen::Success)
    {
        return llt.solve(identityLike(p));
    }

    Eigen::LDLT<Matrix> ldlt(p);
    if (ldlt.info() == Eigen::Success)
    {
        return ldlt.solve(identityLike(p));
    }

    return svdInverse(p);
}

Matrix svdInverse(const Matrix &p)
{
    Eigen::JacobiSVD<Matrix> svd(p, Eigen::ComputeFullU | Eigen::ComputeFullV);
    const Eigen::VectorXd &singular_values = svd.singularValues();

    double tolerance = singular_values(0) * std::max(p.rows(), p.cols()) * 1e-12;

    Eigen::VectorXd inv_singular_values(singular_values.size());
    for (int i = 0; i < singular_values.size(); ++i)
    {
        inv_singular_values(i) = (singular_values(i) > tolerance) ? 1.0 / singular_values(i) : 0.0;
    }

    return svd.matrixV() *
           inv_singular_values.asDiagonal() *
           svd.matrixU().transpose();
}

Matrix ols(const Matrix &p_A, const Matrix &p_L, const Matrix &p_P, std::function<Matrix(const Matrix &)> p_inv_func)
{
    if (p_P.isIdentity() || isPlaceHolder(p_P))
    {
        return p_A.colPivHouseholderQr().solve(p_L);
    }
    else
    {
        Matrix AtPAinv = p_inv_func(normalizedEquation(p_A, p_P));
        Matrix AtPL = p_A.transpose() * p_P * p_L;
        return AtPAinv * AtPL;
    }
}

Matrix xRotation(double p_omega)
{
    const double
        sinw = std::sin(p_omega),
        cosw = std::cos(p_omega);
    Matrix Rw(3, 3);
    Rw << 1, 0, 0,
        0, cosw, -sinw,
        0, sinw, cosw;
    return Rw;
}

Matrix yRotation(double p_phi)
{
    const double
        sinp = std::sin(p_phi),
        cosp = std::cos(p_phi);
    Matrix Rp(3, 3);
    Rp << cosp, 0, -sinp,
        0, 1, 0,
        sinp, 0, cosp;
    return Rp;
}

Matrix zRotation(double p_kappa)
{
    const double
        sink = std::sin(p_kappa),
        cosk = std::cos(p_kappa);
    Matrix Rk(3, 3);
    Rk << cosk, -sink, 0,
        sink, cosk, 0,
        0, 0, 1;
    return Rk;
}

Matrix rotationMatrix(double p_x, double p_y, double p_z)
{
    return xRotation(p_x) * yRotation(p_y) * zRotation(p_z);
}

namespace internal
{
    void validateCoordinateMatrix(const Matrix &p_coord)
    {
        if (!isValidCoordinate(p_coord))
        {
            matrixBadShape(p_coord.rows(), p_coord.cols(), 3, -1);
        }
    }

    void validateRotationMatrix(const Matrix &p_rotate)
    {
        if (!isValidRotationMatrix(p_rotate))
        {
            matrixBadShape(p_rotate.rows(), p_rotate.cols(), 3, 3);
        }
    }
}

Matrix translate(const Matrix &p_coord, double p_x, double p_y, double p_z)
{
    internal::validateCoordinateMatrix(p_coord);

    Matrix translated(p_coord.rows(), p_coord.cols());

    translated.col(0) = p_coord.col(0).array() - p_x;
    translated.col(1) = p_coord.col(1).array() - p_y;
    translated.col(2) = p_coord.col(2).array() - p_z;

    return translated;
}

Matrix rotateForward(const Matrix &p_coord, const Matrix &p_rotate)
{
    internal::validateCoordinateMatrix(p_coord);
    internal::validateRotationMatrix(p_rotate);
    return p_coord * p_rotate;
}

Matrix rotateInverse(const Matrix &p_coord, const Matrix &p_rotate)
{
    internal::validateCoordinateMatrix(p_coord);
    internal::validateRotationMatrix(p_rotate);
    return p_coord * p_rotate.transpose();
}

Matrix scale(const Matrix &p_coord, double p_s)
{
    internal::validateCoordinateMatrix(p_coord);
    return (p_coord.array() * p_s).matrix();
}

double rmse(
    const Matrix &p_v,
    int n,
    int t,
    const Matrix &p_P)
{
    if (isPlaceHolder(p_P) || p_P.isIdentity())
    {
        return ((p_v.transpose() * p_v) / (n - t)).cwiseSqrt()(0);
    }
    else
    {
        return ((p_v.transpose() * p_P * p_v) / (n - t)).cwiseSqrt()(0);
    }
}

Matrix errorMatrix(double p_rmse, const Matrix &p_n)
{
    return p_rmse * p_n.cwiseSqrt();
}

namespace fmt
{
    const Eigen::IOFormat
        python(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]"),
        csv(Eigen::FullPrecision, 0, ", ", "\n", "", "", "", "");
}

M_libga_end