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

    size_t dim_in() const;
    size_t dim_out() const;
    boost::dynamic_bitset<> get_index_mask() const;
    boost::dynamic_bitset<> get_sign_mask() const;

    const FeatureTransform & transform(const std::vector<double> &, std::vector<double> &) const;
    std::shared_ptr<FeatureVector> transform(const std::shared_ptr<FeatureVector> &) const;

    const FeatureTransform & dump_to_ptree(boost::property_tree::ptree &) const;

private:
    size_t dim_in_;
    size_t dim_out_;
    boost::dynamic_bitset<> index_mask_;
    boost::dynamic_bitset<> sign_mask_;  /// false="+", true="-"

    template <typename ContainerType>
    void transform_impl(const ContainerType &, ContainerType &) const;
};

FeatureTransform::FeatureTransform() :
dim_in_(0), dim_out_(0), index_mask_(0), sign_mask_(0) {
}

FeatureTransform::FeatureTransform(const boost::dynamic_bitset<> & index_mask) :
        dim_in_(0), dim_out_(0), index_mask_(0), sign_mask_(0) {
    set_index_mask(index_mask);
}

FeatureTransform::FeatureTransform(const boost::dynamic_bitset<> & index_mask, const boost::dynamic_bitset<> & sign_mask) :
        dim_in_(0), dim_out_(0), index_mask_(0), sign_mask_(0) {
    set_index_mask(index_mask);
    set_sign_mask(sign_mask);
}


FeatureTransform::FeatureTransform(const boost::property_tree::ptree & pt) :
dim_in_(0), dim_out_(0), index_mask_(0), sign_mask_(0) {
    set_index_mask(pt.get<boost::dynamic_bitset<>>("index_mask"));
    set_sign_mask(pt.get<boost::dynamic_bitset<>>("sign_mask"));
    if ( dim_in_!=pt.get<size_t>("dim_in") || dim_out_!=pt.get<size_t>("dim_out") )
        throw std::runtime_error("Cannot create feature transform object - non consistent property tree");
}

FeatureTransform &FeatureTransform::set_index_mask(const boost::dynamic_bitset<> & index_mask) {
    index_mask_ = index_mask;
    if (sign_mask_.empty()) sign_mask_.resize(index_mask_.size(), false);
    dim_in_ = index_mask_.size();
    dim_out_ = index_mask_.count();
    if (sign_mask_.size() != dim_in_)
        throw std::runtime_error("The sizes of index and size masks must coincide");
    return *this;
}

FeatureTransform &FeatureTransform::set_sign_mask(const boost::dynamic_bitset<> & sign_mask) {
    sign_mask_ = sign_mask;
    if (!index_mask_.empty() && sign_mask_.size() != index_mask_.size())
        throw std::runtime_error("The sizes of index and size masks must coincide");
    return *this;
}

size_t FeatureTransform::dim_in() const {
    return dim_in_;
}

size_t FeatureTransform::dim_out() const {
    return dim_out_;
}

boost::dynamic_bitset<> FeatureTransform::get_index_mask() const {
    return index_mask_;
}

boost::dynamic_bitset<> FeatureTransform::get_sign_mask() const {
    return sign_mask_;
}

template<typename ContainerType>
void FeatureTransform::transform_impl(const ContainerType & c_in, ContainerType & c_out) const {
    if (c_in.size() != dim_in_)
        throw std::runtime_error("Expecting an input container of different dimension");
    if (c_out.size() != dim_out_)
        throw std::runtime_error("Expecting an output container of different dimension");
    size_t counter = 0;
    for( size_t i = index_mask_.find_first(); i < dim_in_; i = index_mask_.find_next(i) ) {
        if (sign_mask_[i]) {
            c_out[counter] = - c_in[i];
        } else {
            c_out[counter] = c_in[i];
        }
        counter++;
    }
    if (counter != dim_out_)
        throw std::runtime_error("Something wrong with dimension - check dim_out");
}

const FeatureTransform &FeatureTransform::transform(const std::vector<double> & v_in, std::vector<double> & v_out) const {
    v_out.resize(dim_out_);
    transform_impl(v_in,v_out);
    return *this;
}

std::shared_ptr<FeatureVector> FeatureTransform::transform(const std::shared_ptr<FeatureVector> & pFV_in) const {
    auto pFV_out = std::make_shared<FeatureVector>(dim_out_);
    transform_impl(pFV_in->data_,pFV_out->data_);
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
