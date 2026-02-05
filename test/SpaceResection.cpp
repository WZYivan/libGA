#include <sstream>
#include <print>

#include <lga/Photogrammetry>

using namespace lga;

int main()
{
    Matrix img(4, 2), obj(4, 3), all(4, 5);
    all << -86.15, -68.99, 36589.41, 25273.32, 2195.17,
        -53.40, 82.21, 37631.08, 31324.51, 728.69,
        -14.78, -76.63, 39100.97, 24934.98, 2386.50,
        10.46, 64.43, 40426.54, 30319.81, 757.31;
    img = all.leftCols(2);
    obj = all.rightCols(3);
    img /= 1000;

    Interior interior{
        .x = 0,
        .y = 0,
        .f = 153.24 / 1000,
        .m = 50000};

    Space_Resection_Result result =
        spaceResection(interior, img, obj);

    if (result.info != Iterative_Algo_Info::Success)
    {
        std::println("{:s}", "Failed");
    }

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
        result.exterior.x,
        result.exterior.y,
        result.exterior.z,
        result.exterior.phi,
        result.exterior.omega,
        result.exterior.kappa);
}