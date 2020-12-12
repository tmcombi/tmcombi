#ifndef LIB_TRAIN_CLASSIFIER_H_
#define LIB_TRAIN_CLASSIFIER_H_

#include "classifier.h"
#include "sample.h"

class TrainClassifier {
public:
    explicit TrainClassifier(std::shared_ptr<Sample>);

    virtual void train() = 0;

    virtual std::shared_ptr<Classifier> get_classifier() const = 0;
    virtual void dump_configuration_to_ptree(boost::property_tree::ptree &) const = 0;

protected:
    const std::shared_ptr<Sample> pSample_;
};

TrainClassifier::TrainClassifier(std::shared_ptr<Sample> pSample) : pSample_(std::move(pSample)) {
}

#endif