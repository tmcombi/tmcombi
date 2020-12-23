#ifndef LIB_FEATURE_TRANSFORM_SUBSET_H_
#define LIB_FEATURE_TRANSFORM_SUBSET_H_

#include <boost/dynamic_bitset.hpp>
#include "feature_transform.h"

class FeatureTransformSubset : public FeatureTransform {
public:
    FeatureTransformSubset();
    explicit FeatureTransformSubset(const boost::dynamic_bitset<> &);
    explicit FeatureTransformSubset(const boost::dynamic_bitset<> &, const boost::dynamic_bitset<> &);
    explicit FeatureTransformSubset(const boost::property_tree::ptree &);

    FeatureTransformSubset & set_index_mask(const boost::dynamic_bitset<> &);
    FeatureTransformSubset & set_sign_mask(const boost::dynamic_bitset<> &);     /// false="+", true="-"

    boost::dynamic_bitset<> get_index_mask() const;
    boost::dynamic_bitset<> get_sign_mask() const;

    const FeatureTransformSubset &
    transform_std_vector(const std::vector<double> &, std::vector<double> &) const override;

    const FeatureTransformSubset & dump_to_ptree(boost::property_tree::ptree &) const override;

private:
    boost::dynamic_bitset<> index_mask_;
    boost::dynamic_bitset<> sign_mask_;  /// false="+", true="-"
};

FeatureTransformSubset::FeatureTransformSubset() :
index_mask_(0), sign_mask_(0) {
}

FeatureTransformSubset::FeatureTransformSubset(const boost::dynamic_bitset<> & index_mask) :
index_mask_(0), sign_mask_(0) {
    set_index_mask(index_mask);
}

FeatureTransformSubset::FeatureTransformSubset(const boost::dynamic_bitset<> & index_mask, const boost::dynamic_bitset<> & sign_mask) :
index_mask_(0), sign_mask_(0) {
    set_index_mask(index_mask);
    set_sign_mask(sign_mask);
}


FeatureTransformSubset::FeatureTransformSubset(const boost::property_tree::ptree & pt) :
index_mask_(0), sign_mask_(0) {
    set_index_mask(pt.get<boost::dynamic_bitset<>>("index_mask"));
    set_sign_mask(pt.get<boost::dynamic_bitset<>>("sign_mask"));
    if ( dim_in_!=pt.get<size_t>("dim_in") || dim_out_!=pt.get<size_t>("dim_out") )
        throw std::runtime_error("Cannot create feature transform object - non consistent property tree");
}

FeatureTransformSubset &FeatureTransformSubset::set_index_mask(const boost::dynamic_bitset<> & index_mask) {
    index_mask_ = index_mask;
    if (sign_mask_.empty()) sign_mask_.resize(index_mask_.size(), false);
    dim_in_ = index_mask_.size();
    dim_out_ = index_mask_.count();
    if (sign_mask_.size() != dim_in_)
        throw std::runtime_error("The sizes of index and size masks must coincide");
    return *this;
}

FeatureTransformSubset &FeatureTransformSubset::set_sign_mask(const boost::dynamic_bitset<> & sign_mask) {
    sign_mask_ = sign_mask;
    if (!index_mask_.empty() && sign_mask_.size() != index_mask_.size())
        throw std::runtime_error("The sizes of index and size masks must coincide");
    return *this;
}

boost::dynamic_bitset<> FeatureTransformSubset::get_index_mask() const {
    return index_mask_;
}

boost::dynamic_bitset<> FeatureTransformSubset::get_sign_mask() const {
    return sign_mask_;
}

const FeatureTransformSubset & FeatureTransformSubset::
transform_std_vector(const std::vector<double> & v_in, std::vector<double> & v_out) const {
    v_out.resize(dim_out_);
    if (v_in.size() != dim_in_)
        throw std::runtime_error("Expecting an input container of different dimension");
    size_t counter = 0;
    for( size_t i = index_mask_.find_first(); i < dim_in_; i = index_mask_.find_next(i) ) {
        if (sign_mask_[i]) {
            v_out[counter] = - v_in[i];
        } else {
            v_out[counter] = v_in[i];
        }
        counter++;
    }
    if (counter != dim_out_)
        throw std::runtime_error("Something wrong with dimension - check dim_out");
    return *this;
}

const FeatureTransformSubset &FeatureTransformSubset::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    pt.put("dim_in", dim_in_);
    pt.put("dim_out", dim_out_);
    pt.put("index_mask",index_mask_);
    pt.put("sign_mask",sign_mask_);
    return *this;
}

#endif
