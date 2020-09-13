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
    pFV_.push_back(pFV);
}

#endif
