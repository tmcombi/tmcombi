#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <map>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "feature_vector.h"

class Sample {
public:
    explicit Sample(unsigned int); // unsigned int = dimension
    explicit Sample(const boost::property_tree::ptree &);

    virtual unsigned int push(const std::shared_ptr<FeatureVector>& );

    virtual unsigned int push_no_check(const std::shared_ptr<FeatureVector>& );

    unsigned int get_dim() const;
    unsigned int get_size() const;

    virtual const std::pair<double, double> & get_neg_pos_counts() const;
    const std::shared_ptr<FeatureVector> & operator[](unsigned int) const;
    bool contains(const std::shared_ptr<FeatureVector> &) const;

    // has no item greater than any of another sample
    bool operator<=(const Sample &) const;
    // has no item smaller than any of another sample
    bool operator>=(const Sample &) const;
    bool has_no_intersection_with(const Sample &) const;

    const Sample & dump_to_ptree(boost::property_tree::ptree &) const;

    const boost::dynamic_bitset<> & get_lower_border();
    const boost::dynamic_bitset<> & get_upper_border();


protected:
    std::pair<double, double> total_neg_pos_counts_;

private:
    void compute_borders();

    const unsigned int dim_;
    bool pushed_without_check_;
    std::vector<std::shared_ptr<FeatureVector>> pFV_;
    std::map<const std::vector<double>,unsigned int> fv2index_map_;

    boost::dynamic_bitset<> lower_border_;
    boost::dynamic_bitset<> upper_border_;
    bool borders_computed_;
};

Sample::Sample(unsigned int dim):
total_neg_pos_counts_(0,0), dim_(dim),
pushed_without_check_(false), borders_computed_(false) {
}

unsigned int Sample::get_dim() const {
    return dim_;
}

unsigned int Sample::get_size() const {
    return pFV_.size();
}

unsigned int Sample::push(const std::shared_ptr<FeatureVector>& pFV) {
    borders_computed_ = false;
    unsigned int index = get_size();
    if (pushed_without_check_)
        throw std::domain_error("Pushing with check does not make sense after you pushed without check!");
    const std::map<const std::vector<double>,unsigned int>::const_iterator it = fv2index_map_.find(pFV->get_data());
    if ( it == fv2index_map_.end() ) {
        fv2index_map_[pFV->get_data()] = pFV_.size();
        pFV_.push_back(pFV);
    }  else {
        index = it->second;
        pFV_[index]->inc_weight_negatives(pFV->get_weight_negatives());
        pFV_[index]->inc_weight_positives(pFV->get_weight_positives());
    }
    total_neg_pos_counts_.first += pFV->get_weight_negatives();
    total_neg_pos_counts_.second += pFV->get_weight_positives();
    return index;
}

unsigned int Sample::push_no_check(const std::shared_ptr<FeatureVector> & pFV) {
    borders_computed_ = false;
    pushed_without_check_ = true;
    pFV_.push_back(pFV);
    total_neg_pos_counts_.first += pFV->get_weight_negatives();
    total_neg_pos_counts_.second += pFV->get_weight_positives();
    return get_size() - 1;
}

const std::shared_ptr<FeatureVector>& Sample::operator[](unsigned int i) const {
    return pFV_[i];
}

const std::pair<double, double> & Sample::get_neg_pos_counts() const {
    return total_neg_pos_counts_;
}

bool Sample::contains(const std::shared_ptr<FeatureVector> & pFV) const {
    if (pushed_without_check_)
        throw std::domain_error("Checking on existence does not make sense after you pushed without check!");
    return fv2index_map_.find(pFV->get_data()) != fv2index_map_.end();
}

bool Sample::operator<=(const Sample & sample) const {
    if (get_dim() != sample.get_dim())
        throw std::domain_error("Unexpected error: trying to compare samples of different dimensions!");
    for ( unsigned int i=0; i < get_size(); ++i )
        for ( unsigned int j=0; j < sample.get_size(); ++j )
            if ( *this->operator[](i) > *sample[j] ) return false;
    return true;
}

bool Sample::operator>=(const Sample & sample) const {
    if (get_dim() != sample.get_dim())
        throw std::domain_error("Unexpected error: trying to compare samples of different dimensions!");
    for ( unsigned int i=0; i < get_size(); ++i )
        for ( unsigned int j=0; j < sample.get_size(); ++j )
            if ( *this->operator[](i) < *sample[j] ) return false;
    return true;
}

bool Sample::has_no_intersection_with(const Sample & sample) const {
    if (get_dim() != sample.get_dim())
        throw std::domain_error("Unexpected error: trying to compare samples of different dimensions!");
    for ( unsigned int i=0; i < get_size(); ++i )
        if (sample.contains((this->operator[](i))))
            return false;
    return true;
}

const Sample &Sample::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const unsigned int dim = get_dim();
    const unsigned int size = get_size();
    pt.put("dim", dim);
    pt.put("size", size);
    ptree children;
    for (unsigned int i = 0; i < size; ++i) {
        ptree child;
        operator[](i)->dump_to_ptree(child);
        children.push_back(std::make_pair("", child));
    }
    pt.add_child("feature_vectors", children);
    pt.put("total_neg", get_neg_pos_counts().first);
    pt.put("total_pos", get_neg_pos_counts().second);
    return *this;
}

Sample::Sample(const boost::property_tree::ptree & pt)
: dim_(pt.get<double>("dim")), pushed_without_check_(false), borders_computed_(false)
{
    const unsigned int size = pt.get<double>("size");
    for (auto& item : pt.get_child("feature_vectors")) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>(item.second);
        Sample::push(pFV);
    }
    if (size != get_size())
        throw std::domain_error("Error during parsing of json-ptree: given sample size does not correspond to the feature vector count!");
}

void Sample::compute_borders() {
    const unsigned int size = get_size();
    lower_border_ = boost::dynamic_bitset<>(size);
    upper_border_ = boost::dynamic_bitset<>(size);
    for (unsigned int i = 0; i < size; ++i)
        for (unsigned int j = i+1; j < size; ++j) {
            const bool i_smaller_j = *(operator[](i)) < *(operator[](j));
            const bool j_smaller_i = *(operator[](j)) < *(operator[](i));
            upper_border_[i] |= i_smaller_j;
            upper_border_[j] |= j_smaller_i;
            lower_border_[i] |= j_smaller_i;
            lower_border_[j] |= i_smaller_j;
        }
    lower_border_.flip();upper_border_.flip();
    borders_computed_ = true;
}

const boost::dynamic_bitset<> &Sample::get_lower_border() {
    if (!borders_computed_)
        compute_borders();
    return lower_border_;
}

const boost::dynamic_bitset<> &Sample::get_upper_border() {
    if (!borders_computed_)
        compute_borders();
    return upper_border_;
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
