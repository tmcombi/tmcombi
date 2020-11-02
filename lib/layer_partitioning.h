#ifndef LIB_LAYER_PARTITIONING_H_
#define LIB_LAYER_PARTITIONING_H_

#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "layer.h"
#include "graph_creator.h"

class LayerPartitioning {
public:
    LayerPartitioning();

    explicit LayerPartitioning(const boost::property_tree::ptree &);

    unsigned int dim() const;
    unsigned int size() const;
    bool consistent() const;

    LayerPartitioning & push_back(const std::shared_ptr<Sample> &);

    /// create two completely new layers instead of input layer (not on place)
    /// input iterator points to the second new layer
    void split_layer(std::deque<std::shared_ptr<Layer> >::iterator&, const boost::dynamic_bitset<> &);

    /// merge (on place) data from the second layer to the first layer and delete the second layer from partitioning
    /// input iterator points to the merged layer
    void merge_two_layers(const std::deque<std::shared_ptr<Layer> >::const_iterator&);

    std::deque<std::shared_ptr<Layer>>::iterator begin();
    std::deque<std::shared_ptr<Layer>>::iterator end();

    std::deque<std::shared_ptr<Layer>>::const_reverse_iterator rbegin() const;
    std::deque<std::shared_ptr<Layer>>::const_reverse_iterator rend() const;

    const LayerPartitioning & dump_to_ptree(boost::property_tree::ptree &);

    typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
            boost::no_property,
            boost::property<boost::edge_capacity_t, double,
                    boost::property<boost::edge_residual_capacity_t, double,
                            boost::property<boost::edge_reverse_t, Traits::edge_descriptor> > > > GraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    std::shared_ptr<GraphType> get_graph(const std::shared_ptr<Layer> &) const;

private:
    unsigned int dim_;
    std::deque<std::shared_ptr<Layer> > pLayer_;
    SampleCreator sample_creator_;
    std::map<std::shared_ptr<Layer>, std::shared_ptr<GraphType> > layer2graph_map_;
};

LayerPartitioning::LayerPartitioning() : dim_(0) {}

LayerPartitioning &LayerPartitioning::push_back(const std::shared_ptr<Sample> & pSample) {
    if (!dim_)
        dim_=pSample->dim();
    else
        if (dim_!=pSample->dim())
            throw std::runtime_error("Layer partitioning cannot include parts of different dimensions");
    pLayer_.push_back(pSample);
#ifdef TIMERS
    const std::clock_t time1 = std::clock();
#endif
    const auto pGraphCreator = std::make_shared<GraphCreator<GraphType, AuxTrGraphType> >(pSample);
#ifdef TIMERS
    const std::clock_t time2 = std::clock();
    std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Create starting graph" << std::endl;
#endif
    pGraphCreator->do_transitive_reduction();
#ifdef TIMERS
    const std::clock_t time3 = std::clock();
    std::cout << "Timers: " << (time3-time2)/(CLOCKS_PER_SEC/1000) << "ms - Transitive reduction of the starting graph" << std::endl;
#endif
    layer2graph_map_[pSample]=pGraphCreator->get_graph();
    return *this;
}

unsigned int LayerPartitioning::dim() const {
    return dim_;
}

unsigned int LayerPartitioning::size() const {
    return pLayer_.size();
}

bool LayerPartitioning::consistent() const {
    if (pLayer_.empty()) return true;
#ifdef TIMERS
    const std::clock_t time1 = std::clock();
#endif
    if (pLayer_.size() != layer2graph_map_.size()) return false;
    auto previous = pLayer_.begin();
    for(auto it = previous + 1; it != pLayer_.end(); ++it) {
        if ( !(**previous <= **it) ) return false;
        previous = it;
    }
    for(auto it1 = pLayer_.begin(); it1 != pLayer_.end(); ++it1) {
        for(auto it2 = it1 +1; it2 != pLayer_.end(); ++it2) {
            if (!(*it1)->has_no_intersection_with(**it2)) return false;
        }
    }
    for(const auto & it : pLayer_) {
        auto pGraphCreator = std::make_shared<GraphCreator<GraphType, AuxTrGraphType> >(it);
        pGraphCreator->do_transitive_reduction();
        auto pGraph2 = pGraphCreator->get_graph();
        auto pGraph1 = layer2graph_map_.at(it);
        if (boost::num_vertices(*pGraph1) != boost::num_vertices(*pGraph2))
            return false;
        const unsigned int ne1 = boost::num_edges(*pGraph1);
        const unsigned int ne2 = boost::num_edges(*pGraph2);
        if (ne1 != ne2)
            return false;
        boost::graph_traits<GraphType>::edge_iterator ei, ei_end;
        for (std::tie(ei, ei_end) = boost::edges(*pGraph1); ei!=ei_end; ++ei) {
            auto s = boost::source(*ei,*pGraph1);
            auto t = boost::target(*ei,*pGraph1);
            auto edge_pair = boost::edge(s,t,*pGraph2);
            if (!edge_pair.second) return false;
        }
        for (std::tie(ei, ei_end) = boost::edges(*pGraph2); ei!=ei_end; ++ei) {
            auto s = boost::source(*ei,*pGraph2);
            auto t = boost::target(*ei,*pGraph2);
            auto edge_pair = boost::edge(s,t,*pGraph1);
            if (!edge_pair.second) return false;
        }
    }
#ifdef TIMERS
    const std::clock_t time2 = std::clock();
    std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Consistency check of the layer partitioning" << std::endl;
#endif
    return true;
}

std::deque<std::shared_ptr<Layer>>::iterator LayerPartitioning::begin() {
    return pLayer_.begin();
}

std::deque<std::shared_ptr<Layer>>::iterator LayerPartitioning::end() {
    return pLayer_.end();
}

std::deque<std::shared_ptr<Layer>>::const_reverse_iterator LayerPartitioning::rbegin() const {
    return pLayer_.rbegin();
}

std::deque<std::shared_ptr<Layer>>::const_reverse_iterator LayerPartitioning::rend() const {
    return pLayer_.rend();
}

void LayerPartitioning::split_layer(
        std::deque<std::shared_ptr<Layer>>::iterator & it, const boost::dynamic_bitset<> & mask
        ) {
    std::shared_ptr<Layer> pLower;
    std::shared_ptr<Layer> pUpper;
    std::tie(pLower,pUpper) = sample_creator_.split_sample(*it, mask);
    const auto pGraph = layer2graph_map_[*it];
    const auto pGraphCreatorUpper = std::make_shared<GraphCreator<GraphType , AuxTrGraphType> >(pGraph, mask);
    boost::dynamic_bitset<> maskLower(mask);
    maskLower.flip();
    const auto pGraphCreatorLower = std::make_shared<GraphCreator<GraphType , AuxTrGraphType> >(pGraph, maskLower);
    layer2graph_map_.erase(*it);
    it = pLayer_.erase(it);
    it = pLayer_.insert(it,pUpper);
    layer2graph_map_[*it] = pGraphCreatorUpper->get_graph();
    it = pLayer_.insert(it,pLower);
    layer2graph_map_[*it] = pGraphCreatorLower->get_graph();
    ++it;
}

void LayerPartitioning::merge_two_layers(
        const std::deque<std::shared_ptr<Layer>>::const_iterator & it
        ) {
    const auto it2 = it+1;
    const unsigned int size1 = (*it)->size();
    const unsigned int size2 = (*it2)->size();
    (*it)->push(*it2);
    const auto pGraph1 = layer2graph_map_[*it];
    const auto pGraph2 = layer2graph_map_[*it2];
    for(unsigned int i = 0; i<size2; i++) {
        boost::add_vertex(*pGraph1);
    }
    boost::graph_traits<GraphType>::edge_iterator ei, ei_end;
    for (std::tie(ei, ei_end) = boost::edges(*pGraph2); ei!=ei_end; ++ei) {
        const auto s = boost::source(*ei,*pGraph2);
        const auto t = boost::target(*ei,*pGraph2);
        boost::add_edge(size1+s,size1+t,*pGraph1);
    }
    for(unsigned int i = 0; i<size1; i++) {
        for(unsigned int j = 0; j<size2; j++) {
            if (*((**it)[i]) <= *((**it2)[j]))
                boost::add_edge(i,size1+j,*pGraph1);
        }
    }
    const auto pGraphCreator = std::make_shared<GraphCreator<GraphType , AuxTrGraphType> >(pGraph1);
    pGraphCreator->do_transitive_reduction();
    layer2graph_map_.erase(*it2);
    pLayer_.erase(it2);
}

std::shared_ptr<LayerPartitioning::GraphType> LayerPartitioning::get_graph(const std::shared_ptr<Layer> & pLayer) const {
    const auto it = layer2graph_map_.find(pLayer);
    if (layer2graph_map_.find(pLayer) == layer2graph_map_.end())
        throw std::runtime_error("unexpected error");
    return it->second;
}

const LayerPartitioning & LayerPartitioning::dump_to_ptree(boost::property_tree::ptree & pt) {
    using boost::property_tree::ptree;
    const unsigned int dim = this->dim();
    const unsigned int size = this->size();
    pt.put("dim", dim);
    pt.put("size", size);
    ptree children;
    for (auto & it : *this) {
        ptree child;
        it->dump_to_ptree(child);
        children.push_back(std::make_pair("", child));
    }
    pt.add_child("layers", children);
    return *this;
}

LayerPartitioning::LayerPartitioning(const boost::property_tree::ptree & pt) : dim_(pt.get<double>("dim")) {
    const unsigned int size = pt.get<double>("size");
    for (auto& item : pt.get_child("layers")) {
        std::shared_ptr<Sample> pSample = std::make_shared<Sample>(item.second);
        push_back(pSample);
    }
    if (size != this->size())
        throw std::domain_error("Error during parsing of json-ptree:"
                                "given layer partitioning size does not correspond to the layer count!");
}


#endif
