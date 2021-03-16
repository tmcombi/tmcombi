#ifndef LIB_FEATURE_TRANSFORM_SUBSET_H_
#define LIB_FEATURE_TRANSFORM_SUBSET_H_

#include "feature_mask.h"
#include "feature_transform.h"

class FeatureTransformSubset : public FeatureTransform {
public:
    FeatureTransformSubset();

    explicit FeatureTransformSubset(const std::shared_ptr<FeatureMask> &);
    FeatureTransformSubset & set_feature_mask(const std::shared_ptr<FeatureMask> &);

    std::shared_ptr<const FeatureMask> get_feature_mask() const;

    const FeatureTransformSubset &
    transform_std_vector(const std::vector<double> &, std::vector<double> &) const override;

    const FeatureTransformSubset & dump_to_ptree(boost::property_tree::ptree &) const override;
    explicit FeatureTransformSubset(const boost::property_tree::ptree &);

    //todo: remove deprecated
    FeatureTransformSubset(const boost::dynamic_bitset<> &);
    FeatureTransformSubset(const boost::dynamic_bitset<> &, const boost::dynamic_bitset<> &);
    FeatureTransformSubset & set_index_mask(const boost::dynamic_bitset<> &);
    FeatureTransformSubset & set_sign_mask(const boost::dynamic_bitset<> &);
    boost::dynamic_bitset<> get_index_mask();
    boost::dynamic_bitset<> get_sign_mask();

private:
    std::shared_ptr<FeatureMask> pFM_;
};

FeatureTransformSubset::FeatureTransformSubset() : FeatureTransform(), pFM_(std::make_shared<FeatureMask>()) {
}

FeatureTransformSubset::FeatureTransformSubset(const std::shared_ptr<FeatureMask> & pFM) {
    set_feature_mask(pFM);
}

FeatureTransformSubset &FeatureTransformSubset::set_feature_mask(const std::shared_ptr<FeatureMask> & pFM) {
    pFM_ = pFM;
    dim_in_ = pFM_->dim();
    dim_out_ = pFM_->count();
    return *this;
}

std::shared_ptr<const FeatureMask> FeatureTransformSubset::get_feature_mask() const {
    return pFM_;
}

const FeatureTransformSubset & FeatureTransformSubset::
transform_std_vector(const std::vector<double> & v_in, std::vector<double> & v_out) const {
    v_out.resize(dim_out_);
    if (v_in.size() != dim_in_)
        throw std::runtime_error("Expecting an input container of different dimension");
    size_t counter = 0;
    for( size_t i = pFM_->find_first(); i < dim_in_; i = pFM_->find_next(i) ) {
        if (pFM_->sign(i)) {
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

FeatureTransformSubset::FeatureTransformSubset(const boost::property_tree::ptree & pt) : pFM_(nullptr) {
    if ( pt.get<std::string>("type") != "FeatureTransformSubset" )
        throw std::runtime_error("Expecting configuration of type FeatureTransformSubset");
    const auto pFM = std::make_shared<FeatureMask>(pt.get_child("feature_mask"));
    set_feature_mask(pFM);
    if ( dim_in_!=pt.get<size_t>("dim_in") || dim_out_!=pt.get<size_t>("dim_out") )
        throw std::runtime_error("Cannot create feature transform object - non consistent property tree");
}

const FeatureTransformSubset &FeatureTransformSubset::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    pt.put("type", "FeatureTransformSubset");
    pt.put("dim_in", dim_in_);
    pt.put("dim_out", dim_out_);
    ptree fm;
    pFM_->dump_to_ptree(fm);
    pt.add_child("feature_mask", fm);
    return *this;
}

FeatureTransformSubset::FeatureTransformSubset(const boost::dynamic_bitset<> & bs) :
FeatureTransform(), pFM_(std::make_shared<FeatureMask>()) {
    set_index_mask(bs);
}

FeatureTransformSubset::FeatureTransformSubset(const boost::dynamic_bitset<> & bs1, const boost::dynamic_bitset<> & bs2) :
        FeatureTransform(), pFM_(std::make_shared<FeatureMask>()) {
    set_index_mask(bs1);
    set_sign_mask(bs2);
}

FeatureTransformSubset &FeatureTransformSubset::set_index_mask(const boost::dynamic_bitset<> & bs) {
    pFM_->resize(bs.size());
    dim_in_ = pFM_->dim();
    for ( size_t i = 0; i < pFM_->dim(); i++ ) {
        (*pFM_)[i] = bs[i];
    }
    dim_out_ = pFM_->count();
    return *this;
}

FeatureTransformSubset &FeatureTransformSubset::set_sign_mask(const boost::dynamic_bitset<> & bs) {
    if ( pFM_->dim() != bs.size() )
        throw std::runtime_error("Unexpected error");
    for ( size_t i = 0; i < pFM_->dim(); i++ ) {
        (*pFM_).sign(i) = bs[i];
    }
    return *this;
}

boost::dynamic_bitset<> FeatureTransformSubset::get_index_mask() {
    return pFM_->get_index_mask();
}

boost::dynamic_bitset<> FeatureTransformSubset::get_sign_mask() {
    return pFM_->get_sign_mask();
}

#endif
