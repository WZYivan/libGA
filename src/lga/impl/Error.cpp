#include <lga/Error>

M_libga_begin

    M_libga_impl_error_with_msg_only(Not_Implement_Error, "Not implemented");

Matrix_Shape_Error::Matrix_Shape_Error() noexcept
    : m_prefix("Matrix shape doesn't match expection") {}
Matrix_Shape_Error::Matrix_Shape_Error(const std::string &p_msg, int p_r, int p_c, int p_r_e, int p_c_e) noexcept
    : m_prefix(p_msg), m_row(p_r), m_col(p_c), m_row_e(p_r_e), m_col_e(p_c_e) {}
Matrix_Shape_Error::Matrix_Shape_Error(const char *p_msg, int p_r, int p_c, int p_r_e, int p_c_e) noexcept
    : m_prefix(p_msg), m_row(p_r), m_col(p_c), m_row_e(p_r_e), m_col_e(p_c_e) {}
const char *Matrix_Shape_Error::what() const noexcept
{
    m_msg = std::format("{:s}: expect ({:d},{:d}), get ({:d},{:d})",
                        m_prefix, m_row_e, m_col_e, m_row, m_col);
    return m_msg.c_str();
}

M_libga_end