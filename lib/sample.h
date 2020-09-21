#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <map>
#include <vector>
#include "feature_vector.h"

class Sample {
public:
    // unsigned int = dimension
    explicit Sample(unsigned int);

    void push(const std::shared_ptr<FeatureVector>& );

    unsigned int get_dim() const;
    unsigned int get_size() const;
    const std::pair<double, double> & get_neg_pos_counts();
    const std::shared_ptr<FeatureVector> & operator[](unsigned int) const;
    int find(const std::shared_ptr<FeatureVector> &) const;

private:
    const unsigned int dim_;
    std::vector<std::shared_ptr<FeatureVector>> pFV_;
    std::map<const std::vector<double>,unsigned int> fv2index_map_;
    std::pair<double, double> total_neg_pos_counts_;
};

Sample::Sample(unsigned int dim): dim_(dim), total_neg_pos_counts_(0,0) {
}

unsigned int Sample::get_dim() const {
    return dim_;
}

unsigned int Sample::get_size() const {
    return pFV_.size();
}

void Sample::push(const std::shared_ptr<FeatureVector>& pFV) {
    //todo: use iterator and avoid searching two times the same object
    if ( fv2index_map_.find(pFV->get_data()) == fv2index_map_.end() ) {
        fv2index_map_[pFV->get_data()] = pFV_.size();
        pFV_.push_back(pFV);
    }  else {
        const unsigned int offset = fv2index_map_[pFV->get_data()];
        pFV_[offset]->inc_weight_negatives(pFV->get_weight_negatives());
        pFV_[offset]->inc_weight_positives(pFV->get_weight_positives());
    }
    total_neg_pos_counts_.first += pFV->get_weight_negatives();
    total_neg_pos_counts_.second += pFV->get_weight_positives();
}

const std::shared_ptr<FeatureVector>& Sample::operator[](unsigned int i) const {
    return pFV_[i];
}

const std::pair<double, double> & Sample::get_neg_pos_counts() {
    return total_neg_pos_counts_;
}

int Sample::find(const std::shared_ptr<FeatureVector> & pFV) const {
    //todo: use iterator and avoid searching two times the same object
    //todo: create test
    if ( fv2index_map_.find(pFV->get_data()) == fv2index_map_.end() ) {
        return -1;
    }
    return fv2index_map_.at(pFV->get_data());
}

std::ostream &operator<<(std::ostream & stream, const Sample & sample) {
    if (sample.get_size()) {
        stream << *sample[0];
    }
    for (unsigned int i = 1; i < sample.get_size(); ++i) {
        stream << ';' << *sample[i];
    }
    return stream;
}


#endif
