#include <map>
#include <vector>
#include <string>

#include <boost/property_map/property_map.hpp>

#include <lga/OfficialWork>

using N = lga::Elev_Net;
using NT = lga::Net_Traits<N>;

int main()
{
    N net;

    NT::Vertex_Descriptor
        a = boost::add_vertex(NT::Vertex_Property{"A", 237.483, true}, net),
        b = boost::add_vertex(NT::Vertex_Property{"B"}, net),
        c = boost::add_vertex(NT::Vertex_Property{"C"}, net),
        d = boost::add_vertex(NT::Vertex_Property{"D"}, net);

    boost::add_edge(a, b, NT::Edge_Property{"1", 5.835, 3.5}, net);
    boost::add_edge(b, c, NT::Edge_Property{"2", 3.782, 2.7}, net);
    boost::add_edge(a, c, NT::Edge_Property{"3", 9.640, 4.0}, net);
    boost::add_edge(d, c, NT::Edge_Property{"4", 7.384, 3.0}, net);
    boost::add_edge(a, d, NT::Edge_Property{"5", 2.270, 2.5}, net);

    std::ignore = lga::netAdjust(net);

    return 0;
}