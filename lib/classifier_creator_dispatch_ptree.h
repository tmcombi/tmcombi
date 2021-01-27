#ifndef LIB_CLASSIFIER_CREATOR_DISPATCH_PTREE_H_
#define LIB_CLASSIFIER_CREATOR_DISPATCH_PTREE_H_

#include "classifier_creator.h"
#include "classifier_tmc.h"
#include "classifier_transformed_features.h"
#include "classifier_weighted_sum.h"
#include "feature_transform_subset.h"
#include "feature_transform_superposition.h"

class ClassifierCreatorDispatchPtree : public ClassifierCreator {
public:
    ClassifierCreatorDispatchPtree() = delete;
    explicit ClassifierCreatorDispatchPtree(const boost::property_tree::ptree &);

    std::shared_ptr<Classifier> get_classifier() const override;

private:
    std::shared_ptr<Classifier> pC_;

    static std::shared_ptr<FeatureTransform> feature_transform_dispatch_ptree(const boost::property_tree::ptree & pt) ;
};

ClassifierCreatorDispatchPtree::ClassifierCreatorDispatchPtree(const boost::property_tree::ptree & pt) : pC_(nullptr) {
    auto type = pt.get<std::string>("type");
    if ( type == "ClassifierTmc" ) {
        pC_ = std::make_shared<ClassifierTmc>( std::make_shared<BorderSystem>(pt) );
    } else if ( type == "ClassifierTransformedFeatures" ) {
        const std::shared_ptr<ClassifierCreator> pCC = std::make_shared<ClassifierCreatorDispatchPtree>( pt.get_child("classifier") );
        const auto pFT = feature_transform_dispatch_ptree( pt.get_child("feature_transform") );
        pC_ = std::make_shared<ClassifierTransformedFeatures>(pCC->get_classifier(),pFT);
    } else if ( type == "ClassifierWeightedSum" ) {
        pC_ = std::make_shared<ClassifierWeightedSum>(pt);
    } else {
        throw std::runtime_error("Classifier configuration type is not supported");
    }
}

std::shared_ptr<Classifier> ClassifierCreatorDispatchPtree::get_classifier() const {
    return pC_;
}

std::shared_ptr<FeatureTransform> ClassifierCreatorDispatchPtree::
feature_transform_dispatch_ptree(const boost::property_tree::ptree & pt) {
    std::shared_ptr<FeatureTransform> pFT = nullptr;
    auto type = pt.get<std::string>("type");
    if ( type == "FeatureTransformSubset" ) {
        pFT = std::make_shared<FeatureTransformSubset>(pt);
    } else if ( type == "FeatureTransformSuperposition" ) {
        std::vector<std::shared_ptr<Classifier>> classifiers;
        const auto dim_in = pt.get<size_t>("dim_in");
        const auto dim_out = pt.get<size_t>("dim_out");
        for (auto& item : pt.get_child("classifiers")) {
            std::shared_ptr<Classifier> pC = ClassifierCreatorDispatchPtree(item.second).get_classifier();
            if (pC->dim() != dim_in)
                throw std::runtime_error("All classifiers in the superpositions should accept input of the same dimension");
            classifiers.push_back(pC);
        }
        if ( dim_out!=classifiers.size() )
            throw std::runtime_error("Cannot create feature transform object - non consistent property tree");
        pFT = std::make_shared<FeatureTransformSuperposition>(classifiers);
    } else {
        throw std::runtime_error("feature transform type is not supported");
    }
    return pFT;
}


#endif