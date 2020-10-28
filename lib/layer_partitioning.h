#ifndef LIB_LAYER_PARTITIONING_H_
#define LIB_LAYER_PARTITIONING_H_

#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "layer.h"
#include "graph_creator.h"

class LayerPartitioning {
public:
    LayerPartitioning();

    // todo: create graphs!
    explicit LayerPartitioning(const boost::property_tree::ptree &);

    unsigned int dim() const;
    unsigned int size() const;
    bool consistent() const;

    LayerPartitioning & push_back(const std::shared_ptr<Sample> &);

    std::deque<std::shared_ptr<Layer> >::const_iterator
    split_layer(const std::deque<std::shared_ptr<Layer> >::const_iterator&, const boost::dynamic_bitset<> &);

    std::deque<std::shared_ptr<Layer>>::const_iterator begin() const;
    std::deque<std::shared_ptr<Layer>>::const_iterator end() const;

    std::deque<std::shared_ptr<Layer>>::const_reverse_iterator rbegin() const;
    std::deque<std::shared_ptr<Layer>>::const_reverse_iterator rend() const;

    const LayerPartitioning & dump_to_ptree(boost::property_tree::ptree &) const;

    typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
            boost::no_property,
            boost::property<boost::edge_capacity_t, double,
                    boost::property<boost::edge_residual_capacity_t, double,
                            boost::property<boost::edge_reverse_t, Traits::edge_descriptor> > > > GraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

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
    auto pGraphCreator = std::make_shared<GraphCreator<GraphType, AuxTrGraphType> >(pSample);
    pGraphCreator->do_transitive_reduction();
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
    return true;
}

std::deque<std::shared_ptr<Layer>>::const_iterator LayerPartitioning::begin() const {
    return pLayer_.begin();
}

std::deque<std::shared_ptr<Layer>>::const_iterator LayerPartitioning::end() const {
    return pLayer_.end();
}

std::deque<std::shared_ptr<Layer>>::const_reverse_iterator LayerPartitioning::rbegin() const {
    return pLayer_.rbegin();
}

std::deque<std::shared_ptr<Layer>>::const_reverse_iterator LayerPartitioning::rend() const {
    return pLayer_.rend();
}

std::deque<std::shared_ptr<Layer>>::const_iterator
LayerPartitioning::split_layer(
        const std::deque<std::shared_ptr<Layer>>::const_iterator& it, const boost::dynamic_bitset<> & mask
        ) {
    std::shared_ptr<Layer> pLower;
    std::shared_ptr<Layer> pUpper;
    std::tie(pLower,pUpper) = sample_creator_.split_sample(*it, mask);
    auto pGraph = layer2graph_map_[*it];
    auto pGraphCreatorUpper = std::make_shared<GraphCreator<GraphType , AuxTrGraphType> >(pGraph, mask);
    boost::dynamic_bitset<> maskLower(mask);
    maskLower.flip();
    auto pGraphCreatorLower = std::make_shared<GraphCreator<GraphType , AuxTrGraphType> >(pGraph, maskLower);
    auto it2ins = pLayer_.erase(it);
    it2ins = pLayer_.insert(it2ins,pUpper);
    layer2graph_map_[*it2ins] = pGraphCreatorUpper->get_graph();
    it2ins = pLayer_.insert(it2ins,pLower);
    layer2graph_map_[*it2ins] = pGraphCreatorLower->get_graph();
    return it2ins + 1;
}

const LayerPartitioning &LayerPartitioning::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const unsigned int dim = this->dim();
    const unsigned int size = this->size();
    pt.put("dim", dim);
    pt.put("size", size);
    ptree children;
    for (const auto & it : *this) {
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
        std::shared_ptr<Sample> sample = std::make_shared<Sample>(item.second);
        pLayer_.push_back(sample);
    }
    if (size != this->size())
        throw std::domain_error("Error during parsing of json-ptree:"
                                "given layer partitioning size does not correspond to the layer count!");
}

#endif
