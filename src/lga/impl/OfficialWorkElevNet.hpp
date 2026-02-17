/**
 * @file OfficialWorkElevNet.hpp
 * @author WZYivan (227006975@qq.com)
 * @brief implement of elevation net adjustment
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef M_libga_impl_elev_net
#define M_libga_impl_elev_net

#include <boost/pending/queue.hpp>

#include <lga/OfficialWork>

/**
 * @brief lga global elevation difference observation tag
 *
 */
struct lga_elev_dif_obs_t
{
    using kind = boost::edge_property_tag;
};

/**
 * @brief lga global elevation observation tag
 *
 */
struct lga_elev_obs_t
{
    using kind = boost::vertex_property_tag;
};

namespace boost
{
    /**
     * @brief extend lga_elev_dif_obs_t to boost
     * @see lga_elev_dif_obs_t
     * @see lga::Elev_Net_Edge_Ds
     *
     */
    enum edge_lga_elev_dif_obs_t
    {
        edge_lga_elev_dif_obs
    };

    BOOST_INSTALL_PROPERTY(edge, lga_elev_dif_obs);

    /**
     * @brief extend lga_elev_obs_t to boost
     * @see lga_elev_obs_t
     * @see lga::Elev_Net_Vertex_Ds
     *
     */
    enum vertex_lga_elev_obs_t
    {
        vertex_lga_elev_obs
    };

    BOOST_INSTALL_PROPERTY(vertex, lga_elev_obs);
}

namespace lga
{
    /**
     * @brief data structure of vertex in elev net
     *
     */
    struct Elev_Net_Vertex_Ds
    {
        std::string name;
        double elev = 0.0;
        bool is_control = false;
        bool is_init = is_control;
    };

    /**
     * @brief data structure of edge in elev net
     *
     */
    struct Elev_Net_Edge_Ds
    {
        std::string name;
        double
            diff = 0.0,
            len = 0.0;
    };

    /**
     * @brief make elev net type
     *
     */
    using Elev_Net = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        boost::property<lga_elev_obs_t, Elev_Net_Vertex_Ds>,
        boost::property<lga_elev_dif_obs_t, Elev_Net_Edge_Ds>>;

    /**
     * @brief specialization of Elev_Net
     * @see Elev_Net
     * @tparam
     */
    template <>
    struct Net_Property_Traits<Elev_Net>
    {
        using Vertex_Property_Tag_Type = lga_elev_obs_t;
        using Edge_Property_Tag_Type = lga_elev_dif_obs_t;
        static const Vertex_Property_Tag_Type Vertex_Property_Tag;
        static const Edge_Property_Tag_Type Edge_Property_Tag;
    };

    /**
     * @brief specialization of Elev_Net
     * @see Elev_Net
     * @tparam
     */
    template <>
    struct Net_Property_Display_Traits<Elev_Net_Vertex_Ds, Elev_Net_Edge_Ds>
    {
        static std::string vpFmt(const Elev_Net_Vertex_Ds &vp)
        {
            return std::format("> V[ {} ]: elev = {}, is_control = {}, is_init = {}\n", vp.name, vp.elev, vp.is_control, vp.is_init);
        }
        static std::string epFmt(const Elev_Net_Edge_Ds &ep)
        {
            return std::format("= E[ {} ]: diff = {}, len = {}\n", ep.name, ep.diff, ep.len);
        }
    };

    /**
     * @brief main loop in bfs order of elev net adjustment
     *
     */
    class Elev_Net_Visitor : public boost::bfs_visitor<>
    {
    public:
        using N = Elev_Net;
        using NT = Net_Traits<N>;

        ~Elev_Net_Visitor() = default;
        Elev_Net_Visitor(
            NT::Vertex_Property_Map::type &vert_prop,
            const NT::Edge_Property_Map::type &edge_prop,
            Net_Indices_Manager<Elev_Net> &nim,
            Matrix &A,
            Matrix &P,
            Matrix &L,
            double unit_w)
            : m_self_loop_detected(false),
              m_vert_prop(vert_prop),
              m_edge_prop(edge_prop),
              m_nim(nim),
              m_A(A),
              m_P(P),
              m_L(L),
              m_unit_weight(unit_w)
        {
        }

        /**
         * @brief detect self loop
         *
         * @tparam Edge
         * @tparam Graph
         * @param e
         * @param g
         */
        template <class Edge, class Graph>
        void non_tree_edge(Edge e, Graph &g)
        {
            m_self_loop_detected = true;
        }

        /**
         * @brief initialize each vertex and update matrix(A, L, P) based on each out edge
         *
         * @tparam Edge
         * @tparam Graph
         * @param e
         * @param g
         */
        template <class Edge, class Graph>
        void examine_edge(Edge e, Graph &g)
        {
            const NT::Edge_Property &ep = boost::get(m_edge_prop, e);
            NT::Vertex_Descriptor
                u = boost::source(e, g),
                v = boost::target(e, g);
            NT::Vertex_Property
                &up = boost::get(m_vert_prop, u),
                &vp = boost::get(m_vert_prop, v);

            // init vertex
            if (up.is_init && !vp.is_init)
            {
                vp.is_init = true;
                vp.elev = up.elev + ep.diff;
            }

#if (M_libga_debug)
            std::println("> e = {}, u = {}, v = {}", ei, u, v);
            std::println("> Edge [{}] : {} -> {}", ep.name, up.name, vp.name);
            std::println("\tdiff = {}, len = {}", ep.diff, ep.len);
            std::println("\tu [{}:{}], elev = {}, is_control = {}, is_init = {}", up.name, m_nim.getvi(u), up.elev, up.is_control, up.is_init);
            std::println("\tv [{}:{}], elev = {}, is_control = {}, is_init = {}", vp.name, m_nim.getvi(v), vp.elev, vp.is_control, vp.is_init);
#endif
            // update weight matrix
            m_P(ei, ei) = m_unit_weight / ep.len;
            m_nim.putei(e, ei);

            // update coefficient and residual matrix
            auto A = m_A.row(ei);
            auto L = m_L.row(ei);

            L(0) += ep.diff;
            L(0) += up.elev;
            L(0) -= vp.elev;

            if (!up.is_control)
            {
                A(m_nim.getvi(u)) = -1;
            }
            if (!vp.is_control)
            {
                A(m_nim.getvi(v)) = 1;
            }

            L(0) *= 1000;

            ei += 1;
        }

        bool isValid() const { return !m_self_loop_detected; }

    private:
        bool m_self_loop_detected;
        NT::Vertex_Property_Map::type &m_vert_prop;
        const NT::Edge_Property_Map::type &m_edge_prop;
        Net_Indices_Manager<Elev_Net> &m_nim;
        Matrix &m_A, &m_P, &m_L;
        double m_unit_weight;
        mutable std::size_t ei = 0;
    };

    /**
     * @brief specialization of Elev_Net, adjust the whole net using `Elev_Net_Visitor`
     * @see Elev_Net
     * @see Elev_Net_Visitor
     * @tparam
     */
    template <>
    inline Adjust_Net_Result<Elev_Net> netAdjust(Elev_Net &net, double unit_w)
    {
        using N = Elev_Net;
        using NT = Net_Traits<N>;
        using R = Adjust_Net_Result<Elev_Net>;

        R r;
        r.net = net;

        NT::Vertex_Property_Map::type vert_prop = boost::get(NT::Vertex_Property_Tag, net);
        NT::Edge_Property_Map::type edge_prop = boost::get(NT::Edge_Property_Tag, net);
        Net_Indices_Manager<N> nim{};

        std::vector<NT::Vertex_Descriptor> seeds;
        size_t n = boost::num_edges(net), t = 0, vi = 0;

        // build index of each temporary vertex
        for (auto [it, end] = boost::vertices(net);
             it != end; ++it)
        {
            if (get(vert_prop, *it).is_control)
            {
                seeds.push_back(*it);
                continue;
            }
            t += 1;
            nim.putvi(*it, vi++);
        }

        if (n < t)
        {
            throw std::invalid_argument(std::format("observation({}) < temporary({})", n, t));
        }

        boost::queue<NT::Vertex_Descriptor> buffer;
        std::vector<boost::default_color_type> colors(boost::num_vertices(net), boost::white_color);
        auto vert_color = boost::make_iterator_property_map(
            colors.begin(),
            boost::get(boost::vertex_index, net));
        Matrix
            A = Matrix::Zero(n, t),
            P = Matrix::Zero(n, n),
            L = Matrix::Zero(n, 1);

        Elev_Net_Visitor visitor(vert_prop, edge_prop, nim, A, P, L, unit_w);
        boost::breadth_first_search(net, seeds.begin(), seeds.end(), buffer, visitor, vert_color);

        if (!visitor.isValid())
        {
            throw std::invalid_argument("self loop detected");
        }

        Matrix AtAi = choleskyInverse(normalizedEquation(A, P));
        Matrix AtPL = A.transpose() * P * L;
        Matrix x = AtAi * AtPL / 1000;
        Matrix V = A * x - L / 1000;

#if (M_libga_debug)
        std::cout << "> A:\n"
                  << A.format(fmt::python) << "\n"
                  << "> P:\n"
                  << P.format(fmt::python) << "\n"
                  << "> L:\n"
                  << L.format(fmt::python) << "\n"
                  << "> x:\n"
                  << x.format(fmt::python) << "\n"
                  << "> V:\n"
                  << V.format(fmt::python) << "\n";
#endif

        // apply correction of vertex
        for (auto [it, end] = boost::vertices(net);
             it != end; ++it)
        {
            NT::Vertex_Descriptor vd = *it;
            NT::Vertex_Property &vp = boost::get(vert_prop, vd);
            if (vp.is_control)
            {
                continue;
            }
            vp.elev += x(nim.getvi(vd), 0);
        }

        // apply correction of edge
        for (auto [it, end] = boost::edges(net);
             it != end; ++it)
        {
            NT::Edge_Descriptor ed = *it;
            NT::Edge_Property &ep = boost::get(edge_prop, ed);
            ep.diff += V(nim.getei(ed), 0);
        }

        r.rmse = rmse(V, n, t, P);
        r.sigma = errorMatrix(r.rmse, AtAi);

#if (M_libga_debug)
        {
            boost::queue<NT::Vertex_Descriptor> buffer;
            std::vector<boost::default_color_type> colors(boost::num_vertices(net), boost::white_color);
            auto vert_color = boost::make_iterator_property_map(
                colors.begin(),
                boost::get(boost::vertex_index, net));

            Bfs_Display_Visitor<Elev_Net, std::ostream> visitor(std::cout);
            boost::breadth_first_search(net, seeds.begin(), seeds.end(), buffer, visitor, vert_color);
        }
#endif

        return r;
    }
}

#endif