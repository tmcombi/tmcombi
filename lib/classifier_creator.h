#ifndef LIB_CLASSIFIER_CREATOR_H_
#define LIB_CLASSIFIER_CREATOR_H_

#include "classifier.h"

class ClassifierCreator {
public:
    virtual std::shared_ptr<Classifier> get_classifier() const = 0;
};

#endif