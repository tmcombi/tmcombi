#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include "feature_names.h"

class Sample {
public:
    explicit Sample(const FeatureNames& fn);
    unsigned int get_dim();

private:
    FeatureNames fn_;
};

Sample::Sample(const FeatureNames& fn):fn_(fn) {
}

unsigned int Sample::get_dim() {
    return fn_.get_dim();
}

#endif
