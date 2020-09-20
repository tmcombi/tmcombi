#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <utility>

#include "feature_names.h"
#include "feature_vector.h"

class Sample {
public:
    virtual unsigned int get_dim() const = 0;
    virtual unsigned int get_size() const = 0;
    virtual const std::pair<double, double> &get_neg_pos() = 0;
    virtual const FeatureVector& operator[](unsigned int) const = 0;

};

class SampleContainer : virtual public Sample {
public:
    explicit SampleContainer(std::shared_ptr<FeatureNames> );

    void push(const std::shared_ptr<FeatureVector>& );
    void push_from_stream(std::istream &);

    unsigned int get_dim() const override;
    unsigned int get_size() const override;
    const std::pair<double, double> &get_neg_pos() override;
    const FeatureVector& operator[](unsigned int) const override;

private:
    const std::shared_ptr<FeatureNames> pFN_;
    std::vector<std::shared_ptr<FeatureVector>> pFV_;
    std::map<const std::vector<double>,unsigned int> fv2index_map_;
    std::pair<double, double> total_neg_pos_;
};

SampleContainer::SampleContainer(std::shared_ptr<FeatureNames>  pFN):
pFN_(std::move(pFN)), total_neg_pos_(0,0) {
}

unsigned int SampleContainer::get_dim() const {
    return pFN_->get_dim();
}

unsigned int SampleContainer::get_size() const {
    return pFV_.size();
}

void SampleContainer::push(const std::shared_ptr<FeatureVector>& pFV) {
    if ( fv2index_map_.find(pFV->get_data()) == fv2index_map_.end() ) {
        fv2index_map_[pFV->get_data()] = pFV_.size();
        pFV_.push_back(pFV);
    }  else {
        const unsigned int offset = fv2index_map_[pFV->get_data()];
        pFV_[offset]->inc_weight_negatives(pFV->get_weight_negatives());
        pFV_[offset]->inc_weight_positives(pFV->get_weight_positives());
    }
    total_neg_pos_.first += pFV->get_weight_negatives();
    total_neg_pos_.second += pFV->get_weight_positives();
}

const FeatureVector &SampleContainer::operator[](unsigned int i) const {
    return *pFV_[i];
}

void SampleContainer::push_from_stream(std::istream & is) {
    std::string line;
    while (std::getline(is, line)) {
        line = std::regex_replace (line,std::regex("\r$"),"");
        const std::shared_ptr<FeatureVector> pFV =
                std::make_shared<FeatureVector>(line,
                                                pFN_->get_feature_indices(),
                                                pFN_->get_target_feature_index(),
                                                pFN_->get_negatives_label(),
                                                pFN_->get_positives_label(),
                                                pFN_->get_weight_index());
        push(pFV);
    }
}

const std::pair<double, double> & SampleContainer::get_neg_pos() {
    return total_neg_pos_;
}

class SubSample : virtual public Sample {
public:
    explicit SubSample(std::shared_ptr<Sample>, std::vector<unsigned int>);

    unsigned int get_dim() const override;
    unsigned int get_size() const override;
    const std::pair<double, double> &get_neg_pos() override;
    const FeatureVector& operator[](unsigned int) const override;

private:
    const std::shared_ptr<Sample> pBaseSample_;
    const std::vector<unsigned int> vSubIndex_;
    std::pair<double, double> total_neg_pos_;
    bool total_neg_pos_computed_;
};


SubSample::SubSample(std::shared_ptr<Sample> pBaseSample, std::vector<unsigned int> vSubIndex) :
pBaseSample_(std::move(pBaseSample)), vSubIndex_(std::move(vSubIndex)),
total_neg_pos_(0,0), total_neg_pos_computed_(false) {
}

unsigned int SubSample::get_dim() const {
    return pBaseSample_->get_dim();
}

unsigned int SubSample::get_size() const {
    return vSubIndex_.size();
}

const FeatureVector &SubSample::operator[](unsigned int i) const {
    return pBaseSample_->operator[](vSubIndex_[i]);
}

const std::pair<double, double> &SubSample::get_neg_pos() {
    if (!total_neg_pos_computed_) {
        for(int i = 0; i < get_size(); i++) {
            total_neg_pos_.first +=operator[](i).get_weight_negatives();
            total_neg_pos_.second +=operator[](i).get_weight_positives();
        }
        total_neg_pos_computed_ = true;
    }
    return total_neg_pos_;
}

std::ostream &operator<<(std::ostream & stream, const Sample & sample) {
    if (sample.get_size()) {
        stream << sample[0];
    }
    for (unsigned int i = 1; i < sample.get_size(); ++i) {
        stream << ';' << sample[i];
    }
    return stream;
}

#endif
