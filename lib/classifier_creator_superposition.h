#ifndef LIB_CLASSIFIER_CREATOR_SUPERPOSITION_H_
#define LIB_CLASSIFIER_CREATOR_SUPERPOSITION_H_

#include "classifier_creator_train.h"

/// feature selection - base class

class ClassifierCreatorSuperposition : public ClassifierCreatorTrain {
public:

    ClassifierCreatorSuperposition & set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> &);

protected:
    virtual void reset() = 0;

    std::shared_ptr<ClassifierCreatorTrain> pCCT_;

};

ClassifierCreatorSuperposition &
ClassifierCreatorSuperposition::set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> & pCCT) {
    if (pCCT_ != pCCT) {
        reset();
        pCCT_ = pCCT;
    }
    return *this;
}

#endif