#include <lga/OfficialWork>

int main()
{

    lga::DataFrame df;
    lga::DataFrameColumnIndices indices = {0, 1};
    df.load_index(std::move(indices));
    std::vector<lga::Angle> angles{
        {1, 2, 3},
        {4, 5, 6}};
    df.load_column("Angle", angles);

    lga::config::angle.output_fmt = "{:s} {:03d} d {:02d} m {:02f}";

    df.write<
        std::ostream,
        lga::Angle>(
        std::cout, hmdf::io_format::csv2);
}