#ifndef LIB_CLASSIFIER_CREATOR_FS_H_
#define LIB_CLASSIFIER_CREATOR_FS_H_

#include "classifier_creator_superposition.h"
#include "classifier_transformed_features.h"
#include "feature_selection.h"
#include "feature_transform_subset.h"
#include "sample_creator.h"


/// feature selection - base class

class ClassifierCreatorFs : public ClassifierCreatorSuperposition, public FeatureSelection {
public:
    ClassifierCreatorFs();

    ClassifierCreatorFs & init(const std::shared_ptr<Sample> &) override;

    std::shared_ptr<Classifier> get_classifier() const override;

    ClassifierCreatorFs & train() override;

protected:
    void reset() override;
    virtual void select(const std::shared_ptr<FeatureMask> &) = 0;

private:
    std::shared_ptr<FeatureTransformSubset> pFT_;
    std::shared_ptr<Classifier> pC_;
    bool trained_;
};

ClassifierCreatorFs::ClassifierCreatorFs() : pFT_(nullptr), pC_(nullptr), trained_(false) {
}

ClassifierCreatorFs &ClassifierCreatorFs::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    reset();
    return *this;
}

std::shared_ptr<Classifier> ClassifierCreatorFs::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTransformedFeatures>(pC_,pFT_);
}

ClassifierCreatorFs &ClassifierCreatorFs::train() {
    if ( pCCT_ == nullptr ) throw std::runtime_error("run set_classifier_creator_train() prior to train");
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");
    if ( verbose() ) {
        std::cout << std::endl;
        std::cout << "Starting feature selection" << std::endl;
        double neg, pos;
        std::tie(neg,pos) = pSample->get_neg_pos_counts();
        std::cout << "Input Sample: size=" << pSample->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
    }

    if (pFM_ == nullptr)
        pFM_ = std::make_shared<FeatureMask>(pSample->dim());
    if (pFM_->dim() != pSample->dim())
        throw std::runtime_error("Dimensions of FeatureMask and Sample have to be equal");
    this->select(pFM_);

    if (verbose()) {
        const auto fm_pair = pFM_->to_strings();
        std::cout << std::endl;
        std::cout << "###################################################################################" << std::endl;
        std::cout << "Feature selection finished: ";
        std::cout << "feature_mask = \"" << fm_pair.first << "\", ";
        std::cout << "sign_mask = \"" << fm_pair.second << "\"" << std::endl;
        std::cout << "Training now the classifier with the selected features on the whole sample";
        std::cout << std::endl;
    }

    pFT_ = std::make_shared<FeatureTransformSubset>(pFM_);
    const auto pSampleTransformed = SampleCreator::transform_features(pSample, pFT_);
    (*pCCT_).init(pSampleTransformed).train();
    pC_ = pCCT_->get_classifier();

    trained_ = true;
    return *this;
}

void ClassifierCreatorFs::reset() {
    pFT_ = nullptr;
    pC_ = nullptr;
    trained_ = false;
}

#endif