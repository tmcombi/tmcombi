#ifndef LIB_FEATURE_TRANSFORM_CREATOR_DISPATCH_PTREE_H_
#define LIB_FEATURE_TRANSFORM_CREATOR_DISPATCH_PTREE_H_

#include "feature_transform_subset.h"

class FeatureTransformCreatorDispatchPtree {
public:
    FeatureTransformCreatorDispatchPtree() = delete;
    explicit FeatureTransformCreatorDispatchPtree(const boost::property_tree::ptree &);

    std::shared_ptr<FeatureTransform> get_feature_transform() const;

private:
    std::shared_ptr<FeatureTransform> pFT_;
};

FeatureTransformCreatorDispatchPtree::
FeatureTransformCreatorDispatchPtree(const boost::property_tree::ptree & pt): pFT_(nullptr) {
    auto type = pt.get<std::string>("type");
    if ( type == "FeatureTransformSubset" ) {
        pFT_ = std::make_shared<FeatureTransformSubset>(pt);
    } else if ( type == "foo" ) {
        throw std::runtime_error("place holder");
    } else {
        throw std::runtime_error("feature transform type is not supported");
    }
}

std::shared_ptr<FeatureTransform> FeatureTransformCreatorDispatchPtree::get_feature_transform() const {
    return pFT_;
}

#endif