#ifndef LIB_CLASSIFIER_CREATOR_H_
#define LIB_CLASSIFIER_CREATOR_H_

#include "classifier.h"

class ClassifierCreator {
public:
    ClassifierCreator();

    [[nodiscard]] virtual std::shared_ptr<Classifier> get_classifier() const = 0;

    void verbose(bool);
    [[nodiscard]] bool verbose() const;

    virtual ~ClassifierCreator() = default;

private:
    bool verbose_{};
};

ClassifierCreator::ClassifierCreator() : verbose_(false) {
}

void ClassifierCreator::verbose(bool verbose) {
    verbose_ = verbose;
}

bool ClassifierCreator::verbose() const {
    return verbose_;
}



#endif