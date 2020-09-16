#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <utility>

#include "feature_names.h"
#include "feature_vector.h"

class Sample {
public:
    explicit Sample(std::shared_ptr<FeatureNames> );

    void push(const std::shared_ptr<FeatureVector>& );
    void push_from_stream(std::istream &);

    unsigned int get_dim() const;
    unsigned int get_size() const;
    const FeatureVector& operator[](unsigned int) const;

    friend std::ostream& operator<< (std::ostream&, const Sample&);

private:
    const std::shared_ptr<FeatureNames> pFN_;
    std::vector<std::shared_ptr<FeatureVector>> pFV_;
    std::map<const std::vector<double>,unsigned int> fv2index_map_;
};

Sample::Sample(std::shared_ptr<FeatureNames>  pFN): pFN_(std::move(pFN)) {
}

unsigned int Sample::get_dim() const {
    return pFN_->get_dim();
}

unsigned int Sample::get_size() const {
    return pFV_.size();
}

void Sample::push(const std::shared_ptr<FeatureVector>& pFV) {
    if ( fv2index_map_.find(pFV->get_data()) == fv2index_map_.end() ) {
        fv2index_map_[pFV->get_data()] = pFV_.size();
        pFV_.push_back(pFV);
    }  else {
        const unsigned int offset = fv2index_map_[pFV->get_data()];
        pFV_[offset]->inc_weight_positives(pFV->get_weight_positives());
        pFV_[offset]->inc_weight_negatives(pFV->get_weight_negatives());
    }
}

const FeatureVector &Sample::operator[](unsigned int i) const {
    return *pFV_[i];
}

std::ostream &operator<<(std::ostream & stream, const Sample & sample) {
    if (!sample.pFV_.empty()) {
        stream << *sample.pFV_[0];
    }
    for (unsigned int i = 1; i < sample.pFV_.size(); ++i) {
        stream << ';' << *sample.pFV_[i];
    }
    return stream;
}

void Sample::push_from_stream(std::istream & is) {
    std::string line;
    while (std::getline(is, line)) {
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

#endif
