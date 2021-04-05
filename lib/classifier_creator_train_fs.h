#ifndef LIB_CLASSIFIER_CREATOR_TRAIN_FS_H_
#define LIB_CLASSIFIER_CREATOR_TRAIN_FS_H_

#include "classifier_creator_train.h"
#include "classifier_transformed_features.h"
#include "feature_mask.h"
#include "feature_transform_subset.h"
#include "sample_creator.h"


/// feature selection - base class

class ClassifierCreatorTrainFs : public ClassifierCreatorTrain {
public:
    ClassifierCreatorTrainFs();

    ClassifierCreatorTrainFs & init(const std::shared_ptr<Sample> &) override;

    ClassifierCreatorTrainFs & set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> &);

    std::shared_ptr<Classifier> get_classifier() const override;

    ClassifierCreatorTrainFs & train() override;

protected:
    std::shared_ptr<ClassifierCreatorTrain> pCCT_;
    virtual void reset();
    virtual void select(const std::shared_ptr<FeatureMask> &) = 0;

private:
    std::shared_ptr<FeatureMask> pFM_;
    std::shared_ptr<FeatureTransformSubset> pFT_;
    std::shared_ptr<Classifier> pC_;
    bool trained_;
};

ClassifierCreatorTrainFs::ClassifierCreatorTrainFs() : pFM_(nullptr), pFT_(nullptr), pC_(nullptr), trained_(false) {
}

ClassifierCreatorTrainFs &ClassifierCreatorTrainFs::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    reset();
    return *this;
}

ClassifierCreatorTrainFs &
ClassifierCreatorTrainFs::set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> & pCCT) {
    if (pCCT_ != pCCT) {
        pCCT_ = pCCT;
        reset();
    }
    return *this;
}

std::shared_ptr<Classifier> ClassifierCreatorTrainFs::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTransformedFeatures>(pC_,pFT_);
}

ClassifierCreatorTrainFs &ClassifierCreatorTrainFs::train() {
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
        std::cout << "Training now the classifier with the selected features on the whole sample without folding";
        std::cout << std::endl;
    }

    pFT_ = std::make_shared<FeatureTransformSubset>(pFM_);
    const auto pSampleTransformed = SampleCreator::transform_features(pSample, pFT_);
    (*pCCT_).init(pSampleTransformed).train();
    pC_ = pCCT_->get_classifier();

    trained_ = true;
    return *this;
}

void ClassifierCreatorTrainFs::reset() {
    pFT_ = nullptr;
    pC_ = nullptr;
    trained_ = false;
}



#endif