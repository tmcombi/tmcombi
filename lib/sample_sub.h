#ifndef LIB_SAMPLE_SUB_H_
#define LIB_SAMPLE_SUB_H_

#include <vector>
#include "sample.h"

class SampleSub : virtual public Sample {
public:
    explicit SampleSub(std::shared_ptr<Sample>, std::vector<unsigned int>);

    unsigned int get_dim() const override;
    unsigned int get_size() const override;
    const std::pair<double, double> &get_neg_pos() override;
    const std::shared_ptr<FeatureVector>& operator[](unsigned int) const override;

private:
    const std::shared_ptr<Sample> pBaseSample_;
    const std::vector<unsigned int> vSubIndex_;
    std::pair<double, double> total_neg_pos_;
    bool total_neg_pos_computed_;
};


SampleSub::SampleSub(std::shared_ptr<Sample> pBaseSample, std::vector<unsigned int> vSubIndex) :
pBaseSample_(std::move(pBaseSample)), vSubIndex_(std::move(vSubIndex)),
total_neg_pos_(0,0), total_neg_pos_computed_(false) {
}

unsigned int SampleSub::get_dim() const {
    return pBaseSample_->get_dim();
}

unsigned int SampleSub::get_size() const {
    return vSubIndex_.size();
}

const std::shared_ptr<FeatureVector>& SampleSub::operator[](unsigned int i) const {
    return pBaseSample_->operator[](vSubIndex_[i]);
}

const std::pair<double, double> &SampleSub::get_neg_pos() {
    if (!total_neg_pos_computed_) {
        for(int i = 0; i < get_size(); i++) {
            total_neg_pos_.first +=operator[](i)->get_weight_negatives();
            total_neg_pos_.second +=operator[](i)->get_weight_positives();
        }
        total_neg_pos_computed_ = true;
    }
    return total_neg_pos_;
}

#endif
