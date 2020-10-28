#ifndef LIB_GRAPH_CREATOR_H_
#define LIB_GRAPH_CREATOR_H_

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/transitive_reduction.hpp>

#include "layer.h"
#include "less_relation_iterator.h"


template<typename GraphType, typename TrAuxGraphType = GraphType>
class GraphCreator {
public:
    explicit GraphCreator(const std::shared_ptr<Layer> &);
    GraphCreator(std::shared_ptr<GraphType>, const boost::dynamic_bitset<> &);
    void do_transitive_reduction();
    unsigned int size() const;
    unsigned int num_edges() const;

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
    LessRelationIterator<Layer> it, it_end;
    it.set_container(pLayer).set_begin();
    it_end.set_container(pLayer).set_end();
    pGraph_ = std::make_shared<GraphType>(it, it_end, pLayer->size());
}

template<typename GraphType, typename TrAuxGraphType>
GraphCreator<GraphType, TrAuxGraphType>::GraphCreator
(std::shared_ptr<GraphType> pGraph, const boost::dynamic_bitset<> & bs) {
    unsigned int size = bs.size();
    if (size != boost::num_vertices(*pGraph))
        throw std::runtime_error("Bitset should correspond to vertices to create a subgraph");
    std::map<unsigned int, unsigned int> g2new_g_map;
    unsigned int counter = 0;
    for (unsigned int i = 0; i < size; i++) {
        if (bs[i])
            g2new_g_map[i] = counter++;
    }
    pGraph_ = std::make_shared<GraphType>(counter);
    typename boost::graph_traits<GraphType>::out_edge_iterator ei, e_end;
    for (unsigned int i = 0; i < size; i++) {
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
    std::vector<unsigned int> id_map(boost::num_vertices(*pGraph_));
    std::iota(id_map.begin(), id_map.end(), 0);
    TrAuxGraphType tr_aux_graph;
    boost::transitive_reduction(*pGraph_, tr_aux_graph, boost::make_assoc_property_map(g_to_tr), id_map.data());
    boost::remove_edge_if(does_not_exist_in_reduced_graph(*pGraph_, tr_aux_graph, g_to_tr), *pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
unsigned int GraphCreator<GraphType,TrAuxGraphType>::size() const {
    return boost::num_vertices(*pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
unsigned int GraphCreator<GraphType,TrAuxGraphType>::num_edges() const {
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
