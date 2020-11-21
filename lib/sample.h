#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <map>
#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "data_container.h"

class Sample : virtual public DataContainer {
public:
    explicit Sample(unsigned int); // unsigned int = dimension
    explicit Sample(const boost::property_tree::ptree &);

    unsigned int push(const std::shared_ptr<FeatureVector>& ) override;
    unsigned int push(const std::shared_ptr<Sample>& );

    //deprecated
    //unsigned int push_no_check(const std::shared_ptr<FeatureVector>& );
    //unsigned int push_no_check(const std::shared_ptr<Sample>& );

    const std::map<const std::vector<double>,unsigned int> & get_fv2index_map() const ;

    bool contains(const std::shared_ptr<FeatureVector> &) const override;

    bool has_no_intersection_with(const DataContainer &) const override;

    bool weights_int() const;

    const boost::dynamic_bitset<> & get_lower_border();
    const boost::dynamic_bitset<> & get_upper_border();

    /// if graph is available - use the graph
    template <typename GraphType>
    const boost::dynamic_bitset<> & get_lower_border(const std::shared_ptr<GraphType> &);
    template <typename GraphType>
    const boost::dynamic_bitset<> & get_upper_border(const std::shared_ptr<GraphType> &);


private:
    void compute_borders();

    /// if graph is available - use the graph
    template <typename GraphType>
    void compute_borders(const std::shared_ptr<GraphType> &);

    bool weights_int_;
    //bool pushed_without_check_;
    std::map<const std::vector<double>,unsigned int> fv2index_map_;

    boost::dynamic_bitset<> lower_border_;
    boost::dynamic_bitset<> upper_border_;
    bool borders_computed_;
};

Sample::Sample(unsigned int dim):
DataContainer(dim), weights_int_(true),
//pushed_without_check_(false),
borders_computed_(false) {
}

unsigned int Sample::push(const std::shared_ptr<FeatureVector>& pFV) {
    borders_computed_ = false;
    unsigned int index = size();
//    if (pushed_without_check_)
//        throw std::domain_error("Pushing with check does not make sense after you pushed without check!");
    const auto & neg = pFV->get_weight_negatives();
    const auto & pos = pFV->get_weight_positives();
    const std::map<const std::vector<double>,unsigned int>::const_iterator it = fv2index_map_.find(pFV->get_data());
    if ( it == fv2index_map_.end() ) {
        fv2index_map_[pFV->get_data()] = pFV_.size();
        pFV_.push_back(pFV);
    }  else {
        index = it->second;
        pFV_[index]->inc_weight_negatives(neg);
        pFV_[index]->inc_weight_positives(pos);
    }
    total_neg_pos_counts_.first += neg;
    total_neg_pos_counts_.second += pos;
    if (weights_int_) {
        if ( neg != (double)((int)neg) || pos != (double)((int)pos) )
            weights_int_ = false;
    }
    return index;
}

// todo: replace sample with DataContainer
unsigned int Sample::push(const std::shared_ptr<Sample>& pSample) {
    if (dim() != pSample->dim())
        throw std::domain_error("Trying to merge two samples of different dimensions!");
    const unsigned int size2 = pSample->size();
    for (unsigned i = 0; i < size2; i++) {
        push((*pSample)[i]);
    }
    return size();
}

/* //deprecated
unsigned int Sample::push_no_check(const std::shared_ptr<FeatureVector> & pFV) {
    borders_computed_ = false;
    pushed_without_check_ = true;
    pFV_.push_back(pFV);
    total_neg_pos_counts_.first += pFV->get_weight_negatives();
    total_neg_pos_counts_.second += pFV->get_weight_positives();
    return size() - 1;
}

unsigned int Sample::push_no_check(const std::shared_ptr<Sample>& pSample) {
    if (dim() != pSample->dim())
        throw std::domain_error("Trying to merge two samples of different dimensions!");
    const unsigned int size2 = pSample->size();
    for (unsigned i = 0; i < size2; i++) {
        push_no_check((*pSample)[i]);
    }
    return size();
}
 */

const std::map<const std::vector<double>, unsigned int> & Sample::get_fv2index_map() const {
    return fv2index_map_;
}

bool Sample::contains(const std::shared_ptr<FeatureVector> & pFV) const {
//    if (pushed_without_check_)
//        throw std::domain_error("Checking on existence does not make sense after you pushed without check!");
    return fv2index_map_.find(pFV->get_data()) != fv2index_map_.end();
}

bool Sample::has_no_intersection_with(const DataContainer & dc) const {
    if (dim() != dc.dim())
        throw std::domain_error("Unexpected error: trying to compare samples of different dimensions!");
    for ( unsigned int i=0; i < dc.size(); ++i )
        if (contains(dc[i]))
            return false;
    return true;
}

bool Sample::weights_int() const {
    return weights_int_;
}

Sample::Sample(const boost::property_tree::ptree & pt)
: DataContainer(pt.get<double>("dim")), weights_int_(true),
//pushed_without_check_(false),
borders_computed_(false)
{
    const unsigned int size = pt.get<double>("size");
    for (auto& item : pt.get_child("feature_vectors")) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>(item.second);
        Sample::push(pFV);
    }
    if (size != this->size())
        throw std::domain_error("Error during parsing of json-ptree: given sample size does not correspond to the feature vector count!");
}

void Sample::compute_borders() {
    const unsigned int size = this->size();
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

template <typename GraphType>
void Sample::compute_borders(const std::shared_ptr<GraphType> & pGraph) {
    const unsigned int size = this->size();
    lower_border_ = boost::dynamic_bitset<>(size);
    upper_border_ = boost::dynamic_bitset<>(size);
    typename boost::graph_traits<GraphType>::edge_iterator it, it_end;
    for (std::tie(it,it_end) = boost::edges(*pGraph); it!=it_end; ++it) {
        const auto s = boost::source(*it,*pGraph);
        const auto t = boost::target(*it,*pGraph);
        upper_border_[s] = true;
        lower_border_[t] = true;
    }
    lower_border_.flip();upper_border_.flip();
    borders_computed_ = true;
}

template <typename GraphType>
const boost::dynamic_bitset<> &Sample::get_lower_border(const std::shared_ptr<GraphType> & pGraph) {
    if (!borders_computed_)
        compute_borders(pGraph);
    return lower_border_;
}

template <typename GraphType>
const boost::dynamic_bitset<> &Sample::get_upper_border(const std::shared_ptr<GraphType> & pGraph) {
    if (!borders_computed_)
        compute_borders(pGraph);
    return upper_border_;
}

#endif
