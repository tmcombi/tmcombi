#ifndef LIB_FEATURE_SELECTION_H_
#define LIB_FEATURE_SELECTION_H_

#include "feature_mask.h"

/// feature selection - base class

class FeatureSelection {
public:
    FeatureSelection();

    void set_starting_feature_mask(const FeatureMask &);
    std::shared_ptr<const FeatureMask> get_feature_mask();

protected:
    std::shared_ptr<FeatureMask> pFM_;
};

FeatureSelection::FeatureSelection() : pFM_(nullptr) {
}

void FeatureSelection::set_starting_feature_mask(const FeatureMask & fm) {
    pFM_ = std::make_shared<FeatureMask>(fm);
}

std::shared_ptr<const FeatureMask> FeatureSelection::get_feature_mask() {
    return pFM_;
}


#endif