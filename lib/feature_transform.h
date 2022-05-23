#ifndef LIB_FEATURE_TRANSFORM_H_
#define LIB_FEATURE_TRANSFORM_H_

#include <boost/property_tree/ptree.hpp>
#include "feature_vector.h"

class FeatureTransform {
public:
    FeatureTransform();

    [[nodiscard]] virtual size_t dim_in() const;
    [[nodiscard]] virtual size_t dim_out() const;

    virtual const FeatureTransform & transform_std_vector(const std::vector<double> &, std::vector<double> &) const = 0;
    [[nodiscard]] std::shared_ptr<FeatureVector>
    transform_feature_vector(const std::shared_ptr<FeatureVector> &) const;

    virtual const FeatureTransform & dump_to_ptree(boost::property_tree::ptree &) const = 0;

    virtual ~FeatureTransform() = default;

protected:
    size_t dim_in_;
    size_t dim_out_;
};

FeatureTransform::FeatureTransform() : dim_in_(0), dim_out_(0) {}

size_t FeatureTransform::dim_in() const {
    return dim_in_;
}

size_t FeatureTransform::dim_out() const {
    return dim_out_;
}

std::shared_ptr<FeatureVector> FeatureTransform::
transform_feature_vector(const std::shared_ptr<FeatureVector> & pFV_in) const {
    auto pFV_out = std::make_shared<FeatureVector>(dim_out_);
    this->transform_std_vector(pFV_in->data_,pFV_out->data_);
    pFV_out->weight_negatives_ = pFV_in->weight_negatives_;
    pFV_out->weight_positives_ = pFV_in->weight_positives_;
    return pFV_out;
}



#endif
