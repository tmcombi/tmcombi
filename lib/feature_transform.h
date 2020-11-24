#ifndef LIB_FEATURE_TRANSFORM_H_
#define LIB_FEATURE_TRANSFORM_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/dynamic_bitset.hpp>

#include "feature_vector.h"

class FeatureTransform {
public:
    FeatureTransform();
    explicit FeatureTransform(const boost::dynamic_bitset<> &);
    explicit FeatureTransform(const boost::dynamic_bitset<> &, const boost::dynamic_bitset<> &);
    explicit FeatureTransform(const boost::property_tree::ptree &);

    FeatureTransform & set_index_mask(const boost::dynamic_bitset<> &);
    FeatureTransform & set_sign_mask(const boost::dynamic_bitset<> &);     /// false="+", true="-"

    unsigned int dim_in() const;
    unsigned int dim_out() const;
    boost::dynamic_bitset<> get_index_mask() const;
    boost::dynamic_bitset<> get_sign_mask() const;
    const std::vector<double> & get_feature_indices() const;
    const std::vector<bool> & get_feature_signs() const;

    const FeatureTransform & transform(const std::vector<double> &, std::vector<double> &) const;
    std::shared_ptr<FeatureVector> transform(const std::shared_ptr<FeatureVector> &) const;

    const FeatureTransform & dump_to_ptree(boost::property_tree::ptree &) const;

private:
    unsigned int dim_in_;
    unsigned int dim_out_;
    boost::dynamic_bitset<> index_mask_;
    boost::dynamic_bitset<> sign_mask_;
    std::vector<double> feature_indices_;
    std::vector<bool> feature_signs_;    /// false="+", true="-"
};

FeatureTransform::FeatureTransform() :
dim_in_(0), dim_out_(0), index_mask_(0), sign_mask_(0), feature_indices_(0), feature_signs_(0) {
}

FeatureTransform::FeatureTransform(const boost::dynamic_bitset<> & index_mask) :
        dim_in_(0), dim_out_(0), index_mask_(0), sign_mask_(0), feature_indices_(0), feature_signs_(0) {
    set_index_mask(index_mask);
}

FeatureTransform::FeatureTransform(const boost::dynamic_bitset<> & index_mask, const boost::dynamic_bitset<> & sign_mask) :
        dim_in_(0), dim_out_(0), index_mask_(0), sign_mask_(0), feature_indices_(0), feature_signs_(0) {
    set_index_mask(index_mask);
    set_sign_mask(sign_mask);
}


FeatureTransform::FeatureTransform(const boost::property_tree::ptree & pt) :
dim_in_(0), dim_out_(0), index_mask_(0), sign_mask_(0), feature_indices_(0), feature_signs_(0) {
    set_index_mask(pt.get<boost::dynamic_bitset<>>("index_mask"));
    set_sign_mask(pt.get<boost::dynamic_bitset<>>("sign_mask"));
    if ( dim_in_!=pt.get<unsigned int>("dim_in") || dim_out_!=pt.get<unsigned int>("dim_out") )
        throw std::runtime_error("Cannot create feature transform object - non consistent property tree");
}

FeatureTransform &FeatureTransform::set_index_mask(const boost::dynamic_bitset<> & index_mask) {
    index_mask_ = index_mask;
    if (sign_mask_.empty()) sign_mask_.resize(index_mask_.size(), false);
    dim_in_ = index_mask_.size();
    dim_out_ = index_mask_.count();
    if (sign_mask_.size() != dim_in_)
        throw std::runtime_error("The sizes of index and size masks must coincide");
    feature_indices_.clear();
    feature_signs_.clear();
    for (unsigned int i=0; i < dim_in_; i++) {
        if (index_mask_[i]) {
            feature_indices_.push_back(i);
            feature_signs_.push_back(sign_mask_[i]);
        }
    }
    return *this;
}

FeatureTransform &FeatureTransform::set_sign_mask(const boost::dynamic_bitset<> & sign_mask) {
    sign_mask_ = sign_mask;
    if (!index_mask_.empty() && sign_mask_.size() != index_mask_.size())
        throw std::runtime_error("The sizes of index and size masks must coincide");
    if (!index_mask_.empty()){
        feature_signs_.clear();
        for (unsigned int i=0; i < dim_in_; i++) {
            if (index_mask_[i]) {
                feature_signs_.push_back(sign_mask_[i]);
            }
        }
    }
    return *this;
}

unsigned int FeatureTransform::dim_in() const {
    return dim_in_;
}

unsigned int FeatureTransform::dim_out() const {
    return dim_out_;
}

boost::dynamic_bitset<> FeatureTransform::get_index_mask() const {
    return index_mask_;
}

boost::dynamic_bitset<> FeatureTransform::get_sign_mask() const {
    return sign_mask_;
}

const std::vector<double> &FeatureTransform::get_feature_indices() const {
    return feature_indices_;
}

const std::vector<bool> &FeatureTransform::get_feature_signs() const {
    return feature_signs_;
}

const FeatureTransform &FeatureTransform::transform(const std::vector<double> & v_in, std::vector<double> & v_out) const {
    v_out.resize(dim_out_);
    if (v_in.size() != dim_in_)
        throw std::runtime_error("Expecting a vector of different dimension");
    for (unsigned int i = 0; i<dim_out_; i++) {
        if (feature_signs_[i]) {
            v_out[i] = - v_in[feature_indices_[i]];
        } else {
            v_out[i] = v_in[feature_indices_[i]];
        }
    }
    return *this;
}

std::shared_ptr<FeatureVector> FeatureTransform::transform(const std::shared_ptr<FeatureVector> & pFV_in) const {
    if (pFV_in->dim() != dim_in_)
        throw std::runtime_error("Expecting a vector of different dimension");
    std::shared_ptr<FeatureVector> pFV_out = std::make_shared<FeatureVector>(dim_out_);
    for (unsigned int i = 0; i<dim_out_; i++) {
        if (feature_signs_[i]) {
            pFV_out->data_[i] = - (*pFV_in)[feature_indices_[i]];
        } else {
            pFV_out->data_[i] = (*pFV_in)[feature_indices_[i]];
        }
    }
    pFV_out->weight_negatives_ = pFV_in->weight_negatives_;
    pFV_out->weight_positives_ = pFV_in->weight_positives_;
    return pFV_out;
}

const FeatureTransform &FeatureTransform::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    pt.put("dim_in", dim_in_);
    pt.put("dim_out", dim_out_);
    pt.put("index_mask",index_mask_);
    pt.put("sign_mask",sign_mask_);
    return *this;
}

#endif
