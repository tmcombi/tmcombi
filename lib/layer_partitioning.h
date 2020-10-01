#ifndef LIB_LAYER_PARTITIONING_H_
#define LIB_LAYER_PARTITIONING_H_

#include <boost/dynamic_bitset.hpp>
#include "layer.h"

class LayerPartitioning {
public:
    explicit LayerPartitioning(const std::shared_ptr<Sample> &);
    explicit LayerPartitioning(const boost::property_tree::ptree &);

    unsigned int get_dim() const;
    unsigned int get_size() const;
    bool consistent() const;

    std::deque<std::shared_ptr<Layer>>::const_iterator
    split_layer(const std::deque<std::shared_ptr<Layer>>::const_iterator&, const boost::dynamic_bitset<> &);

    std::deque<std::shared_ptr<Layer>>::const_iterator begin() const;
    std::deque<std::shared_ptr<Layer>>::const_iterator end() const;

    const LayerPartitioning & dump_to_ptree(boost::property_tree::ptree &) const;

private:
    const unsigned int dim_;
    std::deque<std::shared_ptr<Layer>> pLayer_;
    SampleCreator sample_creator_;
};

LayerPartitioning::LayerPartitioning(const std::shared_ptr<Sample> & sample) : dim_(sample->get_dim()) {
    pLayer_.push_back(sample);
}

unsigned int LayerPartitioning::get_dim() const {
    return dim_;
}

unsigned int LayerPartitioning::get_size() const {
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

std::deque<std::shared_ptr<Layer>>::const_iterator
LayerPartitioning::split_layer(
        const std::deque<std::shared_ptr<Layer>>::const_iterator& it, const boost::dynamic_bitset<> & mask
        ) {
    std::shared_ptr<Layer> pLower;
    std::shared_ptr<Layer> pUpper;
    std::tie(pLower,pUpper) = sample_creator_.split_sample(*it, mask);
    auto it2ins = pLayer_.erase(it);
    it2ins = pLayer_.insert(it2ins,pUpper);
    it2ins = pLayer_.insert(it2ins,pLower);
    return it2ins + 1;
}

const LayerPartitioning &LayerPartitioning::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const unsigned int dim = get_dim();
    const unsigned int size = get_size();
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
    if (size != get_size())
        throw std::domain_error("Error during parsing of json-ptree:"
                                "given layer partitioning size does not correspond to the layer count!");
}

#endif