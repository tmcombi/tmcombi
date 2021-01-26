#ifndef LIB_CLASSIFIER_TRANSFORMED_FEATURES_H_
#define LIB_CLASSIFIER_TRANSFORMED_FEATURES_H_

#include <utility>

#include "classifier.h"
#include "feature_transform.h"

class ClassifierTransformedFeatures : public Classifier {
public:
    ClassifierTransformedFeatures( std::shared_ptr<const Classifier> ,
            std::shared_ptr<const FeatureTransform> );

    size_t dim() const override;
    double confidence(const std::vector<double> &) const override;
    std::pair<double,double> confidence_interval(const std::vector<double> &) const override;
    std::pair<double,std::pair<double,double>>
    confidence_and_confidence_interval(const std::vector<double> &) const override;

    void dump_to_ptree(boost::property_tree::ptree &) const override;

private:
    std::shared_ptr<const Classifier> pClassifier_;
    std::shared_ptr<const FeatureTransform> pFT_;

};

ClassifierTransformedFeatures::ClassifierTransformedFeatures(std::shared_ptr<const Classifier>  pClassifier,
                                                             std::shared_ptr<const FeatureTransform> pFT) :
        pClassifier_(std::move(pClassifier)), pFT_(std::move(pFT)) {
}

size_t ClassifierTransformedFeatures::dim() const {
    return pFT_->dim_in();
}

double ClassifierTransformedFeatures::confidence(const std::vector<double> & v) const {
    std::vector<double> vt;
    pFT_->transform_std_vector(v,vt);
    return pClassifier_->confidence(vt);
}

std::pair<double, double> ClassifierTransformedFeatures::confidence_interval(const std::vector<double> & v) const {
    std::vector<double> vt;
    pFT_->transform_std_vector(v,vt);
    return pClassifier_->confidence_interval(vt);
}

std::pair<double, std::pair<double, double>>
ClassifierTransformedFeatures::confidence_and_confidence_interval(const std::vector<double> & v) const {
    std::vector<double> vt;
    pFT_->transform_std_vector(v,vt);
    return pClassifier_->confidence_and_confidence_interval(vt);
}

void ClassifierTransformedFeatures::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    pt.put("type", "ClassifierTransformedFeatures");
    ptree ft;
    pFT_->dump_to_ptree(ft);
    pt.add_child("feature_transform", ft);
    ptree cl;
    pClassifier_->dump_to_ptree(cl);
    pt.add_child("classifier", cl);
}

#endif