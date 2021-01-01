#ifndef LIB_CLASSIFIER_CREATOR_TRAIN_H_
#define LIB_CLASSIFIER_CREATOR_TRAIN_H_

#include "classifier_creator.h"
#include "sample.h"

class ClassifierCreatorTrain : public ClassifierCreator {
public:
    ClassifierCreatorTrain();

    virtual ClassifierCreatorTrain & init(const std::shared_ptr<Sample> &);
    std::shared_ptr<Sample> get_sample() const;

    virtual ClassifierCreatorTrain & train() = 0;

    std::shared_ptr<Classifier> get_classifier() const override = 0;

private:
    std::shared_ptr<Sample> pSample_;
};

ClassifierCreatorTrain::ClassifierCreatorTrain() : pSample_(nullptr) {
}

ClassifierCreatorTrain & ClassifierCreatorTrain::init(const std::shared_ptr<Sample> & pSample) {
    pSample_ = pSample;
    return *this;
}

std::shared_ptr<Sample> ClassifierCreatorTrain::get_sample() const {
    return pSample_;
}

#endif