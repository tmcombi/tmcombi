#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include "feature_names.h"

class Sample {
public:
    explicit Sample(FeatureNames *);
    unsigned int get_dim();

private:
    const std::unique_ptr<FeatureNames> pFN_;
    //const std::shared_ptr<FeatureNames> pFN_;
};

Sample::Sample(FeatureNames * const pFN): pFN_(pFN) {
}

unsigned int Sample::get_dim() {
    return pFN_->get_dim();
}

#endif
