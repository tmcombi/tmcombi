#ifndef LIB_LAYER_DECOMPOSITION_H_
#define LIB_LAYER_DECOMPOSITION_H_

#include <boost/dynamic_bitset.hpp>
#include "layer.h"

class LayerDecomposition {
public:
    explicit LayerDecomposition(const std::shared_ptr<Sample> &); // unsigned int = dimension

    unsigned int get_dim() const;
    unsigned int get_size() const;
    bool consistent() const;

    std::deque<std::shared_ptr<Layer>>::const_iterator
    split_layer(const std::deque<std::shared_ptr<Layer>>::const_iterator&, const boost::dynamic_bitset<> &);

    std::deque<std::shared_ptr<Layer>>::const_iterator begin();
    std::deque<std::shared_ptr<Layer>>::const_iterator end();

private:
    const unsigned int dim_;
    std::deque<std::shared_ptr<Layer>> pLayer_;
    SampleCreator sample_creator_;
};

LayerDecomposition::LayerDecomposition(const std::shared_ptr<Sample> & sample) : dim_(sample->get_dim()) {
    pLayer_.push_back(sample);
}

unsigned int LayerDecomposition::get_dim() const {
    return dim_;
}

unsigned int LayerDecomposition::get_size() const {
    return pLayer_.size();
}

bool LayerDecomposition::consistent() const {
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

std::deque<std::shared_ptr<Layer>>::const_iterator LayerDecomposition::begin() {
    return pLayer_.begin();
}

std::deque<std::shared_ptr<Layer>>::const_iterator LayerDecomposition::end() {
    return pLayer_.end();
}

std::deque<std::shared_ptr<Layer>>::const_iterator
LayerDecomposition::split_layer(
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

#endif