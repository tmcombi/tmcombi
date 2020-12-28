#ifndef LIB_CLASSIFIER_CREATOR_TRAIN_H_
#define LIB_CLASSIFIER_CREATOR_TRAIN_H_

#include "classifier_creator.h"
#include "sample.h"

class ClassifierCreatorTrain : public ClassifierCreator {
public:
    explicit ClassifierCreatorTrain(std::shared_ptr<Sample>);

    virtual void train() = 0;

    virtual std::shared_ptr<Classifier> get_classifier() const = 0;

protected:
    const std::shared_ptr<Sample> pSample_;
};

ClassifierCreatorTrain::ClassifierCreatorTrain(std::shared_ptr<Sample> pSample) : pSample_(std::move(pSample)) {
}

#endif