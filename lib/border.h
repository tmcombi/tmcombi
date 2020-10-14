#ifndef LIB_BORDER_H_
#define LIB_BORDER_H_

#include "../../DynDimRTree/RTree.h"
#include "sample.h"

class Border : virtual public Sample {
public:
    explicit Border(unsigned int); // unsigned int = dimension
    explicit Border(const boost::property_tree::ptree &);

    unsigned int push(const std::shared_ptr<FeatureVector>& ) override;
    unsigned int push_no_check(const std::shared_ptr<FeatureVector>& ) override;

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
    static inline bool SearchCallbackFalse(unsigned int id) { return false; }
    bool neg_pos_counts_set_;
    DynDimRTree::RTree<unsigned int, double> rtree_;
    std::vector<double> min_;
    std::vector<double> max_;
};

Border::Border(unsigned int dim) : Sample(dim), neg_pos_counts_set_(false), rtree_(dim),
min_(dim,std::numeric_limits<double>::max()), max_(dim,-std::numeric_limits<double>::max()) {
}

void Border::set_neg_pos_counts(const std::pair<double, double> & np) {
    total_neg_pos_counts_ = np;
    neg_pos_counts_set_ = true;
}

unsigned int Border::push(const std::shared_ptr<FeatureVector> &v) {
    for (unsigned int i=0; i<get_dim(); i++) {
        if ( v->operator[](i) > max_[i] ) max_[i] = v->operator[](i);
        if ( v->operator[](i) < min_[i] ) min_[i] = v->operator[](i);
    }
    const unsigned int index = Sample::push(v);
    rtree_.Insert(v->get_data().data(),v->get_data().data(),index);
    return index;
}

unsigned int Border::push_no_check(const std::shared_ptr<FeatureVector> &v) {
    const unsigned int index = Sample::push_no_check(v);
    rtree_.Insert(v->get_data().data(),v->get_data().data(),index);
    return index;
}

const std::pair<double, double> &Border::get_neg_pos_counts() const {
    if (!neg_pos_counts_set_)
        throw std::domain_error("In case of Border neg_pos_counts should be set before use!");
    return Sample::get_neg_pos_counts();
}

Border::Border(const boost::property_tree::ptree & pt) : Sample(pt), neg_pos_counts_set_(true),
rtree_(pt.get<double>("dim")),
min_(pt.get<double>("dim"),std::numeric_limits<double>::max()),
max_(pt.get<double>("dim"),-std::numeric_limits<double>::max()) {
    total_neg_pos_counts_.first = pt.get<double>("total_neg");
    total_neg_pos_counts_.second = pt.get<double>("total_pos");
}

bool Border::point_above(const std::vector<double> & v, bool fast = true)  const{
    if (fast)
        return point_above_fast(v);
    return point_above_slow(v);
}

bool Border::point_below(const std::vector<double> & v, bool fast = true)  const{
    if (fast)
        return point_below_fast(v);
    return point_below_slow(v);
}

bool Border::point_above_fast(const std::vector<double> & v)  const{
    const unsigned int dim = get_dim();
    for (unsigned int i=0; i<dim; i++) {
        if ( v.at(i) < min_.at(i) ) return false;
    }
    return rtree_.Search(min_.data(), v.data(), SearchCallbackFalse) > 0;
}

bool Border::point_above_slow(const std::vector<double> & v) const{
    const unsigned int size = get_size();
    for (unsigned int i = 0; i < size; ++i) {
        if (*(operator[](i)) <= v) return true;
    }
    return false;
}

bool Border::point_below_fast(const std::vector<double> & v)  const{
    const unsigned int dim = get_dim();
    for (unsigned int i=0; i<dim; i++) {
        if ( v.at(i) > max_.at(i) ) return false;
    }
    return rtree_.Search(v.data(), max_.data(), SearchCallbackFalse) > 0;
}

bool Border::point_below_slow(const std::vector<double> & v)  const{
    const unsigned int size = get_size();
    for (unsigned int i = 0; i < size; ++i) {
        if (*(operator[](i)) >= v) return true;
    }
    return false;
}

bool Border::consistent() const {
    for (unsigned int i = 0; i < get_size(); ++i)
        for (unsigned int j = i+1; j < get_size(); ++j) {
            if (*(operator[](i)) <= *(operator[](j))) return false;
            if (*(operator[](j)) <= *(operator[](i))) return false;
        }
    return true;
}

#endif
