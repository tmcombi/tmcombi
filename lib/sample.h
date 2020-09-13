#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <utility>

#include "feature_names.h"

class Sample {
public:
    explicit Sample(std::shared_ptr<FeatureNames> );
    unsigned int get_dim();

private:
    const std::shared_ptr<FeatureNames> pFN_;
};

Sample::Sample(std::shared_ptr<FeatureNames>  pFN): pFN_(std::move(pFN)) {
}

unsigned int Sample::get_dim() {
    return pFN_->get_dim();
}

#endif
