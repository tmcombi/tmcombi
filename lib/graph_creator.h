#ifndef LIB_GRAPH_CREATOR_H_
#define LIB_GRAPH_CREATOR_H_

#include <numeric>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/transitive_reduction.hpp>

#include "layer.h"

template<typename GraphType, typename TrAuxGraphType = GraphType>
class GraphCreator {
public:
    explicit GraphCreator(const std::shared_ptr<Layer> &);
    explicit GraphCreator(const std::shared_ptr<GraphType> &);
    GraphCreator(std::shared_ptr<GraphType>, const boost::dynamic_bitset<> &);
    void do_transitive_reduction();
    size_t size() const;
    size_t num_edges() const;

    void print() const;

    std::shared_ptr<GraphType> get_graph() const;

private:
    std::shared_ptr<GraphType> pGraph_;

    typedef std::map<typename GraphType::vertex_descriptor, typename TrAuxGraphType::vertex_descriptor>
            Graph2TrAuxGraphMap;

    struct does_not_exist_in_reduced_graph {
        does_not_exist_in_reduced_graph(const GraphType &, const TrAuxGraphType &, const Graph2TrAuxGraphMap &);
        bool operator()(typename boost::graph_traits<GraphType>::edge_descriptor) const;
        const GraphType & g_;
        const TrAuxGraphType & rg_;
        const Graph2TrAuxGraphMap & g2rg_;
    };
};

template<typename GraphType, typename TrAuxGraphType>
GraphCreator<GraphType,TrAuxGraphType>::GraphCreator(const std::shared_ptr<Layer> & pLayer) {
#ifdef TIMERS
    const std::clock_t graph_creation_time1 = std::clock();
#endif
    // 30k ~ 1.4s?
    const size_t size = pLayer->size();
    const size_t dim = pLayer->dim();
    std::vector<boost::dynamic_bitset<>> adjacency_matrix(size);
    std::vector<boost::dynamic_bitset<>> adjacency_matrix_reduced(size);
    for(size_t i = 0; i < size; i++) {
        adjacency_matrix[i].resize(size,true);
    }
    const auto & fv2index_map = pLayer->get_fv2index_map();
    const auto it_end = fv2index_map.end();
    for (size_t feature_index = 0; feature_index < dim; feature_index++) {
        std::multimap<double,size_t> value2index;
        for(auto it = fv2index_map.begin(); it != it_end; ++it) {
            value2index.insert(std::make_pair(it->first[feature_index], it->second));
        }
        if (value2index.empty())
            throw std::runtime_error("Empty multimap not expected");
        boost::dynamic_bitset<> current_reachability(size), nodes2visit(size);
        current_reachability.flip(); nodes2visit.flip();
        std::multimap<double,size_t>::const_iterator eq_range_begin = value2index.begin(), eq_range_end;
        for ( ; eq_range_begin != value2index.end(); eq_range_begin = eq_range_end ) {
            eq_range_end = value2index.upper_bound(eq_range_begin->first);
            for ( auto it = eq_range_begin; it != eq_range_end; ++it ) {
                current_reachability[it->second] = false;
                adjacency_matrix[it->second] &= current_reachability;
                current_reachability[it->second] = true;
                nodes2visit[it->second] = false;
            }
            current_reachability &= nodes2visit;
        }
    }
    for ( size_t i = 0; i < size; i++ ) {
        adjacency_matrix_reduced[i] = adjacency_matrix[i];
        for ( size_t j = adjacency_matrix_reduced[i].find_first(); j < size; j = adjacency_matrix_reduced[i].find_next(j) ) {
            adjacency_matrix_reduced[i] -= adjacency_matrix[j];
        }
    }
    pGraph_ = std::make_shared<GraphType>(size);
    for ( size_t i = 0; i < size; i++ ) {
        for ( size_t j = adjacency_matrix_reduced[i].find_first(); j < size; j = adjacency_matrix_reduced[i].find_next(j) ) {
            boost::add_edge(i,j,*pGraph_);
        }
    }
#ifdef TIMERS
    const std::clock_t graph_creation_time2 = std::clock();
    std::cout << "Timers: " << (graph_creation_time2-graph_creation_time1)/(CLOCKS_PER_SEC/1000);
    std::cout << "ms - Create a graph induced by the relation \"<=\"" << std::endl;
#endif
}

template<typename GraphType, typename TrAuxGraphType>
GraphCreator<GraphType,TrAuxGraphType>::GraphCreator(const std::shared_ptr<GraphType> & pGraph) : pGraph_(pGraph) {
}

template<typename GraphType, typename TrAuxGraphType>
GraphCreator<GraphType, TrAuxGraphType>::GraphCreator
(std::shared_ptr<GraphType> pGraph, const boost::dynamic_bitset<> & bs) {
    size_t size = bs.size();
    if (size != boost::num_vertices(*pGraph))
        throw std::runtime_error("Bitset should correspond to vertices to create a subgraph");
    std::map<size_t, size_t> g2new_g_map;
    size_t counter = 0;
    for (size_t i = 0; i < size; i++) {
        if (bs[i])
            g2new_g_map[i] = counter++;
    }
    pGraph_ = std::make_shared<GraphType>(counter);
    typename boost::graph_traits<GraphType>::out_edge_iterator ei, e_end;
    for (size_t i = 0; i < size; i++) {
        if (bs[i]) {
            for( tie(ei,e_end) = boost::out_edges(i,*pGraph); ei!=e_end; ++ei ) {
                auto v = boost::target(*ei,*pGraph);
                if (bs[v]) {
                    boost::add_edge(g2new_g_map[i], g2new_g_map[v], *pGraph_);
                }
            }
        }
    }
}

template<typename GraphType, typename TrAuxGraphType>
void GraphCreator<GraphType,TrAuxGraphType>::do_transitive_reduction() {
    Graph2TrAuxGraphMap g_to_tr;
    std::vector<size_t> id_map(boost::num_vertices(*pGraph_));
    std::iota(id_map.begin(), id_map.end(), 0);
    TrAuxGraphType tr_aux_graph;
    boost::transitive_reduction(*pGraph_, tr_aux_graph, boost::make_assoc_property_map(g_to_tr), id_map.data());
    boost::remove_edge_if(does_not_exist_in_reduced_graph(*pGraph_, tr_aux_graph, g_to_tr), *pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
size_t GraphCreator<GraphType,TrAuxGraphType>::size() const {
    return boost::num_vertices(*pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
size_t GraphCreator<GraphType,TrAuxGraphType>::num_edges() const {
    return boost::num_edges(*pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
void GraphCreator<GraphType,TrAuxGraphType>::print() const {
    boost::print_graph(*pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
std::shared_ptr<GraphType> GraphCreator<GraphType,TrAuxGraphType>::get_graph() const {
    return pGraph_;
}

template<typename GraphType, typename TrAuxGraphType>
GraphCreator<GraphType, TrAuxGraphType>::does_not_exist_in_reduced_graph::does_not_exist_in_reduced_graph(
        const GraphType & g, const TrAuxGraphType & rg , const GraphCreator::Graph2TrAuxGraphMap &g2rg)
        : g_(g), rg_(rg), g2rg_(g2rg) {
}

template<typename GraphType, typename TrAuxGraphType>
bool GraphCreator<GraphType, TrAuxGraphType>::does_not_exist_in_reduced_graph::operator()(
        typename boost::graph_traits<GraphType>::edge_descriptor e) const {
    const typename boost::graph_traits<TrAuxGraphType>::vertex_descriptor u = g2rg_.at(boost::source(e,g_));
    const typename boost::graph_traits<TrAuxGraphType>::vertex_descriptor v = g2rg_.at(boost::target(e,g_));
    return !boost::edge(u,v,rg_).second;
}

#endif
