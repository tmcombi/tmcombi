#ifndef LIB_GRAPH_CREATOR_H_
#define LIB_GRAPH_CREATOR_H_

#include <numeric>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/transitive_reduction.hpp>

#include "layer.h"
#include "less_relation_iterator.h"


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
#if 0
    // 30k ~ slow
    LessRelationIterator<Layer> it, it_end;
    it.set_container(pLayer).set_begin();
    it_end.set_container(pLayer).set_end();
    pGraph_ = std::make_shared<GraphType>(it, it_end, pLayer->size());
    do_transitive_reduction();
#endif
#if 0
    // 2.5k 1.36s // 30k ~ 50s
    const size_t size = pLayer->size();
    pGraph_ = std::make_shared<GraphType>(size);
    for ( size_t i = 0; i < size; i++ ) {
        for ( size_t j = 0; j < size; j++) {
            if (i == j) continue;
            if (!(*(*pLayer)[i] < *(*pLayer)[j])) continue;
            typename boost::graph_traits<GraphType>::out_edge_iterator ei, e_end;
            bool create_edge = true;
            for( tie(ei,e_end) = boost::out_edges(i,*pGraph_); ei!=e_end; ++ei ) {
                auto k = boost::target(*ei,*pGraph_);
                if ((*(*pLayer)[k] < *(*pLayer)[j])) { create_edge = false; break; }
            }
            if (create_edge) boost::add_edge(i,j,*pGraph_);
        }
    }
    do_transitive_reduction();
#endif
#if 0
    // 30k ~ 45s
    pGraph_ = std::make_shared<GraphType>(pLayer->size());
    const std::map<const std::vector<double>, size_t> & fv2index_map = pLayer->get_fv2index_map();
    const std::map<const std::vector<double>, size_t>::const_iterator it_end = fv2index_map.end();

    for (auto it_i = fv2index_map.begin(); it_i != it_end; ++it_i) {
        const size_t i = it_i->second;
        auto it_j = it_i;
        for (++it_j;it_j != it_end; ++it_j) {
            const size_t j = it_j->second;
            if (!(*(*pLayer)[i] <= *(*pLayer)[j])) continue;
            typename boost::graph_traits<GraphType>::out_edge_iterator ei, e_end;
            bool create_edge = true;
            for( tie(ei,e_end) = boost::out_edges(i,*pGraph_); ei!=e_end; ++ei ) {
                const auto k = boost::target(*ei,*pGraph_);
                if ((*(*pLayer)[k] <= *(*pLayer)[j])) { create_edge = false; break; }
            }
            if (create_edge) boost::add_edge(i,j,*pGraph_);
        }
    }
    do_transitive_reduction();
#endif
#if 1
    // 30k ~ 21s
    const size_t size = pLayer->size();
    pGraph_ = std::make_shared<GraphType>(size);
    const auto & fv2index_map = pLayer->get_fv2index_map();
    const auto it_rend = fv2index_map.rend();
    const auto it_end = fv2index_map.end();
    std::vector<boost::dynamic_bitset<> > reachable(size);

    for (auto it_i = fv2index_map.rbegin(); it_i != it_rend; ++it_i) {
        const size_t i = it_i->second;
        reachable[i].resize(size,false);
        reachable[i][i] = true;
        for (auto it_j = it_i.base();it_j != it_end; ++it_j) {
            const size_t j = it_j->second;
            if (reachable[i][j]) continue;
            if (!(*(*pLayer)[i] <= *(*pLayer)[j])) continue;
            boost::add_edge(i,j,*pGraph_);
            reachable[i] |= reachable[j];
        }
    }
#endif
#if 0
    // 2.5k 1.8s // 30k ~ 102s
    const size_t size = pLayer->size();
    pGraph_ = std::make_shared<GraphType>(size);

    class VisitorCollect : public DynDimRTree::RTree<size_t, double>::Visitor {
    public:
        void visited(const size_t & n) override {visited_.push_back(n);};
        bool resume() override {return true;};
        std::list<size_t> visited_;
    };

    class VisitorMatch : public DynDimRTree::RTree<size_t, double>::Visitor {
    public:
        VisitorMatch(size_t a, size_t b) : found_non_matching_(false), a_(a), b_(b) {}
        void visited(const size_t & n) override { if (n == a_ || n == b_) {} else {found_non_matching_=true;} }
        bool resume() override {return !found_non_matching_;}
        bool found_non_matching_;
    private:
        const size_t a_;
        const size_t b_;
    };

#ifdef TIMERS
    const std::clock_t time1 = std::clock();
#endif
    const size_t dim = pLayer->dim();
    DynDimRTree::RTree<size_t, double> rtree(dim);
    std::vector<double> max(dim,-std::numeric_limits<double>::max());
    for ( size_t i = 0; i < size; i++ ) {
        rtree.Insert((*pLayer)[i]->get_data().data(),(*pLayer)[i]->get_data().data(),i);
        for (size_t j=0; j<dim; j++) {
            if ( (*pLayer)[i]->operator[](j) > max[j] ) max[j] = (*pLayer)[i]->operator[](j);
        }
    }
#ifdef TIMERS
    const std::clock_t time2 = std::clock();
    std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Build up RTree" << std::endl;
#endif
    for ( size_t i = 0; i < size; i++ ) {
        VisitorCollect visitor_collect;
        rtree.Search((*pLayer)[i]->get_data().data(), max.data(), visitor_collect);
        for (auto it = visitor_collect.visited_.begin(); it != visitor_collect.visited_.end(); ++it) {
            const size_t j = *it;
            if (j == i) continue;
            VisitorMatch visitor_match(i,j);
            rtree.Search((*pLayer)[i]->get_data().data(), (*pLayer)[j]->get_data().data(), visitor_match);
            if (!visitor_match.found_non_matching_) {
                boost::add_edge(i,j,*pGraph_);
            }
        }
    }
    // todo: do we need it here?
    do_transitive_reduction();
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
