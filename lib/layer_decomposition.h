#ifndef LIB_LAYER_DECOMPOSITION_H_
#define LIB_LAYER_DECOMPOSITION_H_

#include "layer.h"

class LayerDecomposition {
public:
    explicit LayerDecomposition(unsigned int); // unsigned int = dimension

    unsigned int get_dim() const;
    unsigned int get_size() const;
    bool consistent() const;

    std::deque<std::shared_ptr<Layer>>::const_iterator begin() const;
    std::deque<std::shared_ptr<Layer>>::const_iterator end() const;

private:
    const unsigned int dim_;
    std::deque<std::shared_ptr<Layer>> pLayer_;
};

LayerDecomposition::LayerDecomposition(unsigned int dim) : dim_(dim) {
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

std::deque<std::shared_ptr<Layer>>::const_iterator LayerDecomposition::begin() const {
    return pLayer_.begin();
}

std::deque<std::shared_ptr<Layer>>::const_iterator LayerDecomposition::end() const {
    return pLayer_.end();
}

#endif
