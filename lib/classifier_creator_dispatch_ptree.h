#ifndef LIB_CLASSIFIER_CREATOR_DISPATCH_PTREE_H_
#define LIB_CLASSIFIER_CREATOR_DISPATCH_PTREE_H_

#include "classifier_creator.h"
#include "classifier_tmc.h"
#include "classifier_transformed_features.h"
#include "feature_transform_subset.h"

class ClassifierCreatorDispatchPtree : public ClassifierCreator {
public:
    ClassifierCreatorDispatchPtree() = delete;
    explicit ClassifierCreatorDispatchPtree(const boost::property_tree::ptree &);

    std::shared_ptr<Classifier> get_classifier() const override;

private:
    std::shared_ptr<Classifier> pC_;
};


ClassifierCreatorDispatchPtree::ClassifierCreatorDispatchPtree(const boost::property_tree::ptree & pt) : pC_(nullptr) {
    auto type = pt.get<std::string>("type");
    if ( type == "ClassifierTmc" ) {
        pC_ = std::make_shared<ClassifierTmc>( std::make_shared<BorderSystem>(pt) );
    } else if ( type == "ClassifierTransformedFeatures" ) {
        const std::shared_ptr<ClassifierCreator> pCC = std::make_shared<ClassifierCreatorDispatchPtree>( pt.get_child("classifier") );
        const std::shared_ptr<FeatureTransform> pFT = std::make_shared<FeatureTransformSubset>( pt.get_child("feature_transform") );
        pC_ = std::make_shared<ClassifierTransformedFeatures>(pCC->get_classifier(),pFT);
    } else {
        throw std::runtime_error("Classifier configuration type is not supported");
    }
}

std::shared_ptr<Classifier> ClassifierCreatorDispatchPtree::get_classifier() const {
    return pC_;
}


#endif