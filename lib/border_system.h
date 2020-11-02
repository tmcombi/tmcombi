#ifndef LIB_BORDER_SYSTEM_H_
#define LIB_BORDER_SYSTEM_H_

#include <cmath>
#include "border.h"

class BorderSystem {
public:
    BorderSystem(unsigned int, unsigned int); // <-- dimension, size
    explicit BorderSystem(const boost::property_tree::ptree &);

    unsigned int dim() const;
    unsigned int size() const;
    bool consistent() const;

    const std::shared_ptr<Border> & get_lower(unsigned int) const;
    const std::shared_ptr<Border> & get_upper(unsigned int) const;

    // return the highest lower border lying below the feature vector, and
    //        the lowest  upper border lying above the feature vector
    // use the quality values of these two borders to build the quality believe interval
    // bool parameter specifies whether to use the fast implementation
    std::pair< int, int > containing_borders(const std::vector<double> &, bool);
    std::pair< double, double > confidence_interval(const std::vector<double> &, bool);

    const BorderSystem & dump_to_ptree(boost::property_tree::ptree &) const;

private:
    const unsigned int dim_;
    std::vector<std::shared_ptr<Border>> pLowerBorder_;
    std::vector<std::shared_ptr<Border>> pUpperBorder_;

    std::pair< int, int > containing_borders_slow(const std::vector<double> &);
    std::pair< int, int > containing_borders_fast(const std::vector<double> &);

    friend class BorderSystemCreator;
};

BorderSystem::BorderSystem(const unsigned int dim, const unsigned int size = 0) :
        dim_(dim), pLowerBorder_(size, nullptr), pUpperBorder_(size, nullptr)  {
}

BorderSystem::BorderSystem(const boost::property_tree::ptree & pt) : dim_(pt.get<double>("dim")) {
    const unsigned int size = pt.get<double>("size");
    for (auto& item : pt.get_child("lower_borders")) {
        std::shared_ptr<Border> border = std::make_shared<Border>(item.second);
        pLowerBorder_.push_back(border);
    }
    for (auto& item : pt.get_child("upper_borders")) {
        std::shared_ptr<Border> border = std::make_shared<Border>(item.second);
        pUpperBorder_.push_back(border);
    }
    if ( size != pLowerBorder_.size() || size != pUpperBorder_.size() )
        throw std::domain_error("Error during parsing of json-ptree:"
                                "given border system size does not correspond to the border count!");
}

unsigned int BorderSystem::dim() const {
    return dim_;
}

unsigned int BorderSystem::size() const {
    if (pLowerBorder_.size() != pUpperBorder_.size())
        throw std::domain_error("Lower and upper border counts must be the same!");
    return pLowerBorder_.size();
}

bool BorderSystem::consistent() const {
    for (unsigned int i = 0; i < size(); ++i) {
        if (!pLowerBorder_[i]->consistent()) return false;
        if (!pUpperBorder_[i]->consistent()) return false;
    }
    for (unsigned int i = 0; i < size()-1 ; ++i) {
        if (!(*pLowerBorder_[i] <= *pLowerBorder_[i+1])) return false;
        if (!(*pUpperBorder_[i] <= *pUpperBorder_[i+1])) return false;
        if (!(*pLowerBorder_[i+1] >= *pLowerBorder_[i])) return false;
        if (!(*pUpperBorder_[i+1] >= *pUpperBorder_[i])) return false;
    }
    return true;
}

const std::shared_ptr<Border> &BorderSystem::get_lower(unsigned int i) const {
    return pLowerBorder_[i];
}

const std::shared_ptr<Border> &BorderSystem::get_upper(unsigned int i) const {
    return pUpperBorder_[i];
}

std::pair<int, int> BorderSystem::containing_borders(const std::vector<double> & v, bool fast = true) {
    //activate this very ugly and slow check only in case of problems
    //if (containing_borders_fast(v) != containing_borders_slow(v))
    //    throw std::domain_error("Slow and fast implementations of containing borders yield different results!");
    if (fast)
        return containing_borders_fast(v);
    return containing_borders_slow(v);
}

std::pair<double, double> BorderSystem::confidence_interval(const std::vector<double> & v, bool fast = true) {
    int l=0, u=0;
    unsigned int n=0, p=0;
    double conf_low = 0, conf_up = 0;
    std::tie(l,u) = containing_borders(v,fast);
    if ( l == -1 ) {
        conf_low = 0;
    } else {
        std::tie(n,p) = pLowerBorder_[l]->get_neg_pos_counts();
        conf_low = ((double)p)/((double)(n+p));
    }
    if ( (unsigned int)u == size() ) {
        conf_up = 1;
    } else {
        std::tie(n,p) = pLowerBorder_[u]->get_neg_pos_counts();
        conf_up = ((double)p)/((double)(n+p));
    }
    return {conf_low, conf_up};
}

const BorderSystem &BorderSystem::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const unsigned int dim = this->dim();
    const unsigned int size = this->size();
    pt.put("dim", dim);
    pt.put("size", size);
    ptree lowerBorderChildren;
    ptree upperBorderChildren;
    for (unsigned int i = 0; i < this->size(); ++i) {
        ptree lowerBorderChild;
        ptree upperBorderChild;
        pLowerBorder_[i]->dump_to_ptree(lowerBorderChild);
        pUpperBorder_[i]->dump_to_ptree(upperBorderChild);
        lowerBorderChildren.push_back(std::make_pair("", lowerBorderChild));
        upperBorderChildren.push_back(std::make_pair("", upperBorderChild));
    }
    pt.add_child("lower_borders", lowerBorderChildren);
    pt.add_child("upper_borders", upperBorderChildren);
    return *this;
}

std::pair<int, int> BorderSystem::containing_borders_slow(const std::vector<double> & v) {
    const int size = (int)this->size();
    int l=0, u=size - 1;
    for (;l < size; ++l) if (!pLowerBorder_[l]->point_above(v)) break;
    for (;u >= 0; --u) if (!pUpperBorder_[u]->point_below(v)) break;
    return {l-1,u+1};
}

std::pair<int, int> BorderSystem::containing_borders_fast(const std::vector<double> & v) {
    const int size = (int)this->size();

    //lower borders
    int left = -1, right = size;
    while (right - left > 1) {
        const int middle = (left + right) / 2;
        if (pLowerBorder_[middle]->point_above(v))
            left = middle;
        else
            right = middle;
    }
    const int l = left;

    //upper borders
    left = -1, right = size;
    while (right - left > 1) {
        const int middle = (left + right) / 2;
        if (pUpperBorder_[middle]->point_below(v))
            right = middle;
        else
            left = middle;
    }
    const int u = right;

    return {l,u};
}


#endif
