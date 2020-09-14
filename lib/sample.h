#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <utility>

#include "feature_names.h"
#include "feature_vector.h"

class Sample {
public:
    explicit Sample(std::shared_ptr<FeatureNames> );

    void push(const std::shared_ptr<FeatureVector>& );

    unsigned int get_dim() const;
    unsigned int get_size() const;

private:
    const std::shared_ptr<FeatureNames> pFN_;
    std::vector<std::shared_ptr<FeatureVector>> pFV_;
    std::map<const std::vector<double>,unsigned int> fv2index_map_;
};

Sample::Sample(std::shared_ptr<FeatureNames>  pFN): pFN_(std::move(pFN)) {
}

unsigned int Sample::get_dim() const {
    return pFN_->get_dim();
}

unsigned int Sample::get_size() const {
    return pFV_.size();
}

void Sample::push(const std::shared_ptr<FeatureVector>& pFV) {
    if ( fv2index_map_.find(pFV->get_data()) == fv2index_map_.end() ) {
        fv2index_map_[pFV->get_data()] = pFV_.size();
        pFV_.push_back(pFV);
    }  else {
        const unsigned int offset = fv2index_map_[pFV->get_data()];
        pFV_[offset]->inc_weight_positives(pFV->get_weight_positives());
        pFV_[offset]->inc_weight_negatives(pFV->get_weight_negatives());
    }
}

#endif
