#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <map>
#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "data_container.h"

class Sample : virtual public DataContainer<FeatureVector::WeightType> {
public:
    typedef FeatureVector::WeightType WeightType;

    explicit Sample(size_t); // size_t = dimension
    explicit Sample(const boost::property_tree::ptree &);

    void push(const std::shared_ptr<FeatureVector>& ) override;
    void push(const std::shared_ptr<const DataContainer>& );

    [[nodiscard]] const std::map<const std::vector<double>,size_t> & get_fv2index_map() const ;

    [[nodiscard]] bool contains(const std::shared_ptr<FeatureVector> &) const override;

    [[nodiscard]] bool has_no_intersection_with(const DataContainer &) const override;

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

    std::map<const std::vector<double>,size_t> fv2index_map_;

    boost::dynamic_bitset<> lower_border_;
    boost::dynamic_bitset<> upper_border_;
    bool borders_computed_;
};

Sample::Sample(size_t dim):
DataContainer(dim),borders_computed_(false) {
}

void Sample::push(const std::shared_ptr<FeatureVector>& pFV) {
    borders_computed_ = false;

    const auto & neg = pFV->get_weight_negatives();
    const auto & pos = pFV->get_weight_positives();
    const auto it = fv2index_map_.find(pFV->get_data());
    if ( it == fv2index_map_.end() ) {
        fv2index_map_[pFV->get_data()] = pFV_.size();
        pFV_.push_back(pFV);
    }  else {
        const size_t index = it->second;
        pFV_[index]->inc_weight_negatives(neg);
        pFV_[index]->inc_weight_positives(pos);
    }
    total_neg_pos_counts_.first += neg;
    total_neg_pos_counts_.second += pos;
}

void Sample::push(const std::shared_ptr<const DataContainer>& pDataContainer) {
    if (dim() != pDataContainer->dim())
        throw std::domain_error("Trying to merge two samples of different dimensions!");
    const size_t size2 = pDataContainer->size();
    for (size_t i = 0; i < size2; i++) {
        push((*pDataContainer)[i]);
    }
}

const std::map<const std::vector<double>, size_t> & Sample::get_fv2index_map() const {
    return fv2index_map_;
}

bool Sample::contains(const std::shared_ptr<FeatureVector> & pFV) const {
    return fv2index_map_.find(pFV->get_data()) != fv2index_map_.end();
}

bool Sample::has_no_intersection_with(const DataContainer & dc) const {
    if (dim() != dc.dim())
        throw std::domain_error("Unexpected error: trying to compare samples of different dimensions!");
    for ( size_t i=0; i < dc.size(); ++i )
        if (contains(dc[i]))
            return false;
    return true;
}

Sample::Sample(const boost::property_tree::ptree & pt)
: DataContainer(pt.get<size_t>("dim")),borders_computed_(false)
{
    const auto size = pt.get<size_t>("size");
    for (auto& item : pt.get_child("feature_vectors")) {
        auto pFV = std::make_shared<FeatureVector>(item.second);
        Sample::push(pFV);
    }
    if (size != this->size())
        throw std::domain_error("Error during parsing of json-ptree: given sample size does not correspond to the feature vector count!");
}

void Sample::compute_borders() {
    const size_t size = this->size();
    lower_border_ = boost::dynamic_bitset<>(size);
    upper_border_ = boost::dynamic_bitset<>(size);
    for (size_t i = 0; i < size; ++i)
        for (size_t j = i+1; j < size; ++j) {
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
    const size_t size = this->size();
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
