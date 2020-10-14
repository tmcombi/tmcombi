#define BOOST_TEST_MODULE lib_test_graph
#include <boost/test/included/unit_test.hpp>
#include "graph.h"

#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/graph_utility.hpp>

BOOST_AUTO_TEST_CASE( graph_basics ) {
    using namespace boost;

    typedef adjacency_list_traits < vecS, vecS, directedS > Traits;
    typedef adjacency_list < listS, vecS, directedS,
            property < vertex_name_t, std::string >,
            property < edge_capacity_t, long,
                    property < edge_residual_capacity_t, long,
                            property < edge_reverse_t, Traits::edge_descriptor > > > > Graph;

    Graph g;

    property_map < Graph, edge_capacity_t >::type
            capacity = get(edge_capacity, g);
    property_map < Graph, edge_reverse_t >::type rev = get(edge_reverse, g);
    property_map < Graph, edge_residual_capacity_t >::type
            residual_capacity = get(edge_residual_capacity, g);

    Traits::vertex_descriptor s, t;

    std::string file_name("/home/vmuser/Downloads/max_flow.dat");
    std::ifstream fs_data(file_name);
    if (!fs_data.is_open())
        throw std::runtime_error("Cannot open file: " + file_name);
    read_dimacs_max_flow(g, capacity, rev, s, t, fs_data);
    fs_data.close();

#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
    std::vector<default_color_type> color(num_vertices(g));
  std::vector<Traits::edge_descriptor> pred(num_vertices(g));
  long flow = edmonds_karp_max_flow
    (g, s, t, capacity, residual_capacity, rev, &color[0], &pred[0]);
#else
    long flow = edmonds_karp_max_flow(g, s, t);
#endif

    std::cout << "c  The total flow:" << std::endl;
    std::cout << "s " << flow << std::endl << std::endl;

    std::cout << "c flow values:" << std::endl;
    graph_traits < Graph >::vertex_iterator u_iter, u_end;
    graph_traits < Graph >::out_edge_iterator ei, e_end;
    for (boost::tie(u_iter, u_end) = vertices(g); u_iter != u_end; ++u_iter)
        for (boost::tie(ei, e_end) = out_edges(*u_iter, g); ei != e_end; ++ei)
            if (capacity[*ei] > 0)
                std::cout << "f " << *u_iter << " " << target(*ei, g) << " "
                          << (capacity[*ei] - residual_capacity[*ei]) << std::endl;
    BOOST_CHECK(true);
}
