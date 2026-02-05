#include <print>
#include <iostream>

#include <lga/Photogrammetry>

using namespace lga;

int main()
{
    double f = 150.0 / 1000.0;
    double m = 50000;
    Exterior
        ex_left{4999.770168, 4999.728897, 2000.002353, 0.00021500, 0.02906441, 0.09524706},
        ex_right{5896.828551, 5070.244316, 2030.443250, 0.01443355, 0.04601826, 0.11046904};
    Interior
        in_left{.f = f, .m = m},
        in_right{.f = f, .m = m};
    Matrix img(5, 4);
    img << 51.758, 80.555, -39.953, 78.463,
        14.618, -0.231, -76.006, 0.036,
        49.88, -0.782, -42.201, -1.022,
        86.14, -1.346, -7.706, -2.112,
        48.035, -79.962, -44.438, -79.736;
    img /= 1000.0;
    Image_Meta_Data
        img_meta_left{ex_left, in_left},
        img_meta_right{ex_right, in_right};
    auto result =
        spaceIntersection(
            img_meta_left,
            img.leftCols(2),
            img_meta_right,
            img.rightCols(2));
    Matrix img_left_inv = obj2img(result, ex_left, in_left);
    Matrix img_right_inv = obj2img(result, ex_right, in_right);
    Matrix img_inv(5, 4);
    img_inv.leftCols(2) = img_left_inv;
    img_inv.rightCols(2) = img_right_inv;

    std::cout << "All img\n"
              << img.format(fmt::python) << "\n"
              << "Batch result\n"
              << result.format(fmt::python) << "\n"
              << "All inv img\n"
              << img_inv.format(fmt::python) << std::endl;

    {
        std::vector<Space_Intersection_Ols_Block> params{
            {.meta = img_meta_left, .image = img.leftCols(2)},
            {.meta = img_meta_right, .image = img.rightCols(2)},

        };
        auto result =
            spaceIntersection(params);
        Matrix img_left_inv = obj2img(result.coordinate, ex_left, in_left);
        Matrix img_right_inv = obj2img(result.coordinate, ex_right, in_right);
        Matrix img_inv(5, 4);
        img_inv.leftCols(2) = img_left_inv;
        img_inv.rightCols(2) = img_right_inv;

        std::cout << "All img\n"
                  << img.format(fmt::python) << "\n"
                  << "Batch result\n"
                  << result.coordinate.format(fmt::python) << "\n"
                  << "All inv img\n"
                  << img_inv.format(fmt::python) << std::endl;
    }
}