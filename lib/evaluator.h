#ifndef LIB_EVALUATOR_H_
#define LIB_EVALUATOR_H_

#include "sample.h"
#include "border_system.h"


class Evaluator {
public:
    Evaluator();

    Evaluator & set_sample(const std::shared_ptr<Sample> &);
    Evaluator & set_border_system(const std::shared_ptr<BorderSystem> &);

private:
    std::shared_ptr<Sample> pSample_;
    std::shared_ptr<BorderSystem> pBorderSystem_;
};

Evaluator::Evaluator() : pSample_(nullptr), pBorderSystem_(nullptr) {
}

Evaluator &Evaluator::set_sample(const std::shared_ptr<Sample> & pSample) {
    pSample_ = pSample;
    return *this;
}

Evaluator &Evaluator::set_border_system(const std::shared_ptr<BorderSystem> & pBorderSystem) {
    pBorderSystem_ = pBorderSystem;
    return *this;
}

#endif
