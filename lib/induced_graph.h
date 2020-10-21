#ifndef LIB_INDUCED_GRAPH_H_
#define LIB_INDUCED_GRAPH_H_

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/transitive_reduction.hpp>

#include "sample.h"
#include "less_relation_iterator.h"


template<typename GraphType, typename TrAuxGraphType = GraphType>
class InducedGraph {
public:
    explicit InducedGraph(const std::shared_ptr<Sample> &);
    void do_transitive_reduction();
    unsigned int size() const;
    unsigned int num_edges() const;

    void print() const;

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
InducedGraph<GraphType,TrAuxGraphType>::InducedGraph(const std::shared_ptr<Sample> & pSample) {
    LessRelationIterator<Sample> it, it_end;
    it.set_container(pSample).set_begin();
    it_end.set_container(pSample).set_end();
    pGraph_ = std::make_shared<GraphType>(it, it_end, pSample->size());
}

template<typename GraphType, typename TrAuxGraphType>
void InducedGraph<GraphType,TrAuxGraphType>::do_transitive_reduction() {
    Graph2TrAuxGraphMap g_to_tr;
    std::vector<unsigned int> id_map(boost::num_vertices(*pGraph_));
    std::iota(id_map.begin(), id_map.end(), 0);
    TrAuxGraphType tr_aux_graph;
    boost::transitive_reduction(*pGraph_, tr_aux_graph, boost::make_assoc_property_map(g_to_tr), id_map.data());
    boost::remove_edge_if(does_not_exist_in_reduced_graph(*pGraph_, tr_aux_graph, g_to_tr), *pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
unsigned int InducedGraph<GraphType,TrAuxGraphType>::size() const {
    return boost::num_vertices(*pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
unsigned int InducedGraph<GraphType,TrAuxGraphType>::num_edges() const {
    return boost::num_edges(*pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
void InducedGraph<GraphType,TrAuxGraphType>::print() const {
    boost::print_graph(*pGraph_);
}

template<typename GraphType, typename TrAuxGraphType>
InducedGraph<GraphType, TrAuxGraphType>::does_not_exist_in_reduced_graph::does_not_exist_in_reduced_graph(
        const GraphType & g, const TrAuxGraphType & rg , const InducedGraph::Graph2TrAuxGraphMap &g2rg)
        : g_(g), rg_(rg), g2rg_(g2rg) {
}

template<typename GraphType, typename TrAuxGraphType>
bool InducedGraph<GraphType, TrAuxGraphType>::does_not_exist_in_reduced_graph::operator()(
        typename boost::graph_traits<GraphType>::edge_descriptor e) const {
    const typename boost::graph_traits<TrAuxGraphType>::vertex_descriptor u = g2rg_.at(boost::source(e,g_));
    const typename boost::graph_traits<TrAuxGraphType>::vertex_descriptor v = g2rg_.at(boost::target(e,g_));
    return !boost::edge(u,v,rg_).second;
}

#endif
