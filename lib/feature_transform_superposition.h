#ifndef LIB_FEATURE_TRANSFORM_SUPERPOSITION_H_
#define LIB_FEATURE_TRANSFORM_SUPERPOSITION_H_

#include <boost/dynamic_bitset.hpp>
#include <utility>
#include "classifier.h"
#include "feature_transform.h"

class FeatureTransformSuperposition : public FeatureTransform {
public:
    FeatureTransformSuperposition();
    explicit FeatureTransformSuperposition(std::vector<std::shared_ptr<Classifier>>);

    FeatureTransformSuperposition & set_classifiers(const std::vector<std::shared_ptr<Classifier>> &);

    const std::vector<std::shared_ptr<Classifier>> &  get_classifiers() const;

    const FeatureTransformSuperposition &
    transform_std_vector(const std::vector<double> &, std::vector<double> &) const override;

    const FeatureTransformSuperposition & dump_to_ptree(boost::property_tree::ptree &) const override;

private:
    std::vector<std::shared_ptr<Classifier>> classifiers_;

    void set_dimensions();
};

void FeatureTransformSuperposition::set_dimensions() {
    dim_out_ = classifiers_.size();
    if (classifiers_.empty()) {
        dim_in_ = 0;
    } else {
        dim_in_ = classifiers_[0]->dim();
        for (size_t i = 0; i < dim_out_; i++) {
            if (classifiers_[i]->dim() != dim_in_)
                throw std::runtime_error("All classifiers in the superpositions should accept input of the same dimension");
        }
    }
}

FeatureTransformSuperposition::FeatureTransformSuperposition() : classifiers_(0) {
}

FeatureTransformSuperposition::FeatureTransformSuperposition(std::vector<std::shared_ptr<Classifier>> classifiers) :
classifiers_(std::move(classifiers)) {
    set_dimensions();
}

FeatureTransformSuperposition &FeatureTransformSuperposition::
set_classifiers(const std::vector<std::shared_ptr<Classifier>> & classifiers) {
    classifiers_ = classifiers;
    set_dimensions();
    return *this;
}

const std::vector<std::shared_ptr<Classifier>> & FeatureTransformSuperposition::get_classifiers() const {
    return classifiers_;
}

const FeatureTransformSuperposition & FeatureTransformSuperposition::
transform_std_vector(const std::vector<double> & v_in, std::vector<double> & v_out) const {
    v_out.resize(dim_out_);
    if (v_in.size() != dim_in_)
        throw std::runtime_error("Expecting an input container of different dimension");
    for (size_t i = 0; i < dim_out_; i++) {
        v_out[i] = classifiers_[i]->confidence(v_in);
    }
    return *this;
}

const FeatureTransformSuperposition &FeatureTransformSuperposition::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    pt.put("type", "FeatureTransformSuperposition");
    pt.put("dim_in", dim_in_);
    pt.put("dim_out", dim_out_);
    ptree classifiersChildren;
    for (size_t i = 0; i < dim_out_; i++) {
        ptree classifierChild;
        classifiers_[i]->dump_to_ptree(classifierChild);
        classifiersChildren.push_back(std::make_pair("", classifierChild));
    }
    pt.add_child("classifiers", classifiersChildren);
    return *this;
}

#endif
