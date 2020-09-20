#ifndef LIB_SAMPLE_CONTAINER_H_
#define LIB_SAMPLE_CONTAINER_H_

#include "feature_names.h"
#include "sample.h"

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

#endif
