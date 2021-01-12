#ifndef LIB_BORDER_H_
#define LIB_BORDER_H_

#include "../../DynDimRTree/RTree.h"
#include "data_container.h"

class Border : virtual public DataContainer {
public:
    explicit Border(size_t); // size_t = dimension
    explicit Border(const boost::property_tree::ptree &);

    void push(const std::shared_ptr<FeatureVector>& ) override;

    void set_neg_pos_counts(const std::pair<double, double> &);
    const std::pair<double, double> & get_neg_pos_counts() const override;

    bool point_above(const std::vector<double> &, bool) const;
    bool point_below(const std::vector<double> &, bool) const;

    bool consistent() const;

private:
    bool point_above_fast(const std::vector<double> &) const;
    bool point_above_slow(const std::vector<double> &) const;
    bool point_below_fast(const std::vector<double> &) const;
    bool point_below_slow(const std::vector<double> &) const;
    static inline bool SearchCallbackFalse(size_t) { return false; }
    bool neg_pos_counts_set_;
    DynDimRTree::RTree<size_t, double> rtree_;
    std::vector<double> min_;
    std::vector<double> max_;
};

Border::Border(size_t dim) : DataContainer(dim), neg_pos_counts_set_(false), rtree_(dim),
min_(dim,std::numeric_limits<double>::max()), max_(dim,-std::numeric_limits<double>::max()) {
}

void Border::set_neg_pos_counts(const std::pair<double, double> & np) {
    total_neg_pos_counts_ = np;
    neg_pos_counts_set_ = true;
}

void Border::push(const std::shared_ptr<FeatureVector> &pFV) {
    for (size_t i=0; i<dim(); i++) {
        if ( pFV->operator[](i) > max_[i] ) max_[i] = pFV->operator[](i);
        if ( pFV->operator[](i) < min_[i] ) min_[i] = pFV->operator[](i);
    }
    const size_t index = size();
    pFV_.push_back(pFV);
    rtree_.Insert(pFV->get_data().data(),pFV->get_data().data(),index);
}

const std::pair<double, double> &Border::get_neg_pos_counts() const {
    if (!neg_pos_counts_set_)
        throw std::domain_error("In case of Border neg_pos_counts should be set before use!");
    return DataContainer::get_neg_pos_counts();
}

Border::Border(const boost::property_tree::ptree & pt) : DataContainer(pt.get<double>("dim")),
neg_pos_counts_set_(false),
rtree_(pt.get<double>("dim")),
min_(pt.get<double>("dim"),std::numeric_limits<double>::max()),
max_(pt.get<double>("dim"),-std::numeric_limits<double>::max()) {

    const size_t size = pt.get<double>("size");
    for (auto& item : pt.get_child("feature_vectors")) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>(item.second);
        Border::push(pFV);
    }
    if (size != this->size())
        throw std::domain_error("Error during parsing of json-ptree: given sample size does not correspond to the feature vector count!");

    for (size_t i = 0; i < size; i++){
        const auto & data = operator[](i)->get_data().data();
        rtree_.Insert(data,data,i);
        for (size_t j=0; j<dim(); j++) {
            if ( data[j] > max_[j] ) max_[j] = data[j];
            if ( data[j] < min_[j] ) min_[j] = data[j];
        }
    }
    total_neg_pos_counts_.first = pt.get<double>("total_neg");
    total_neg_pos_counts_.second = pt.get<double>("total_pos");
    neg_pos_counts_set_ = true;
    if (!consistent())
        throw std::domain_error("loaded border is not consistent");
}

bool Border::point_above(const std::vector<double> & v, bool fast = true)  const{
    return (fast) ? point_above_fast(v) : point_above_slow(v);
}

bool Border::point_below(const std::vector<double> & v, bool fast = true)  const{
    return (fast) ? point_below_fast(v) : point_below_slow(v);
}

bool Border::point_above_fast(const std::vector<double> & v)  const{
    const size_t dim = this->dim();
    for (size_t i=0; i<dim; i++) {
        if ( v.at(i) < min_.at(i) ) return false;
    }
    return rtree_.Search(min_.data(), v.data(), SearchCallbackFalse) > 0;
}

bool Border::point_above_slow(const std::vector<double> & v) const{
    const size_t size = this->size();
    for (size_t i = 0; i < size; ++i) {
        if (*(operator[](i)) <= v) return true;
    }
    return false;
}

bool Border::point_below_fast(const std::vector<double> & v)  const{
    const size_t dim = this->dim();
    for (size_t i=0; i<dim; i++) {
        if ( v.at(i) > max_.at(i) ) return false;
    }
    return rtree_.Search(v.data(), max_.data(), SearchCallbackFalse) > 0;
}

bool Border::point_below_slow(const std::vector<double> & v)  const{
    const size_t size = this->size();
    for (size_t i = 0; i < size; ++i) {
        if (*(operator[](i)) >= v) return true;
    }
    return false;
}

bool Border::consistent() const {
    for (size_t i = 0; i < size(); ++i)
        for (size_t j = i+1; j < size(); ++j) {
            if (*(operator[](i)) <= *(operator[](j))) return false;
            if (*(operator[](j)) <= *(operator[](i))) return false;
        }
    return true;
}

#endif
