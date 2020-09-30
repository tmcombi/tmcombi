#ifndef LIB_BORDER_H_
#define LIB_BORDER_H_

#include "sample.h"

// redefine eg_pos_counts vs. class Sample

class Border : virtual public Sample {
public:
    explicit Border(unsigned int); // unsigned int = dimension
    explicit Border(const boost::property_tree::ptree &);

    void set_neg_pos_counts(const std::pair<double, double> &);
    const std::pair<double, double> & get_neg_pos_counts() const override;

    bool point_above(const std::vector<double> &, bool);
    bool point_below(const std::vector<double> &, bool);

private:
    bool point_above_fast(const std::vector<double> &);
    bool point_above_slow(const std::vector<double> &);
    bool point_below_fast(const std::vector<double> &);
    bool point_below_slow(const std::vector<double> &);
    bool neg_pos_counts_set_{};
};

Border::Border(unsigned int dim) : Sample(dim), neg_pos_counts_set_(false) {
}

void Border::set_neg_pos_counts(const std::pair<double, double> & np) {
    total_neg_pos_counts_ = np;
    neg_pos_counts_set_ = true;
}

const std::pair<double, double> &Border::get_neg_pos_counts() const {
    if (!neg_pos_counts_set_)
        throw std::domain_error("In case of Border neg_pos_counts should be set before use!");
    return Sample::get_neg_pos_counts();
}

Border::Border(const boost::property_tree::ptree & pt) : Sample(pt), neg_pos_counts_set_(true) {
    total_neg_pos_counts_.first = pt.get<double>("total_neg");
    total_neg_pos_counts_.second = pt.get<double>("total_pos");
}

bool Border::point_above(const std::vector<double> & v, bool fast = true) {
    if (fast)
        return point_above_fast(v);
    return point_above_slow(v);
}

bool Border::point_below(const std::vector<double> & v, bool fast = true) {
    if (fast)
        return point_below_fast(v);
    return point_below_slow(v);
}

bool Border::point_above_fast(const std::vector<double> & v) {
    //todo: implement
    return point_above_slow(v);
}

bool Border::point_above_slow(const std::vector<double> & v) {
    //todo: implement
    return false;
}

bool Border::point_below_fast(const std::vector<double> & v) {
    //todo: implement
    return point_below_slow(v);
}

bool Border::point_below_slow(const std::vector<double> & v) {
    //todo: implement
    return false;
}



#endif
