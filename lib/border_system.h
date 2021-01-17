#ifndef LIB_BORDER_SYSTEM_H_
#define LIB_BORDER_SYSTEM_H_

#include <cmath>
#include "border.h"

class BorderSystem {
public:
    BorderSystem(size_t, size_t); // <-- dimension, size
    explicit BorderSystem(const boost::property_tree::ptree &);

    size_t dim() const;
    size_t size() const;

    const std::shared_ptr<Border> & get_lower(size_t) const;
    const std::shared_ptr<Border> & get_upper(size_t) const;

    // return the highest lower border lying below the feature vector, and
    //        the lowest  upper border lying above the feature vector
    // use the quality values of these two borders to build the quality believe interval
    // bool parameter specifies whether to use the fast implementation
    std::pair< int, int > containing_borders(const std::vector<double> &) const;

    std::pair< double, double > confidence_interval(const std::vector<double> &) const;
    double confidence(const std::vector<double> &) const;

    std::pair< double, double > confidence_interval(const std::pair<int, int> &) const; // from containing borders
    double confidence(const std::pair<int, int> &) const; // from containing borders

    const BorderSystem & dump_to_ptree(boost::property_tree::ptree &) const;

private:
    const size_t dim_;
    std::vector<std::shared_ptr<Border>> pLowerBorder_;
    std::vector<std::shared_ptr<Border>> pUpperBorder_;
    std::vector<std::pair<double,double>> cumulative_neg_pos_;

    friend class BorderSystemCreator;
};

BorderSystem::BorderSystem(const size_t dim, const size_t size = 0) :
dim_(dim),
pLowerBorder_(size, nullptr), pUpperBorder_(size, nullptr),
cumulative_neg_pos_(size,{0,0})  {
}

BorderSystem::BorderSystem(const boost::property_tree::ptree & pt) :
dim_(pt.get<size_t>("dim")),
pLowerBorder_(0),pUpperBorder_(0),
cumulative_neg_pos_(pt.get<size_t>("size"),{0,0}) {
    const auto size = pt.get<size_t>("size");
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

    double cumulative_neg = 0, cumulative_pos = 0;
    for (size_t i = 0; i < size; i++) {
        const auto neg_pos_counts = pLowerBorder_[i]->get_neg_pos_counts();
        if ( neg_pos_counts != pUpperBorder_[i]->get_neg_pos_counts() )
            throw std::domain_error("Got inconsistent border system from ptree!");
        cumulative_neg += neg_pos_counts.first;
        cumulative_pos += neg_pos_counts.second;
        cumulative_neg_pos_[i] = {cumulative_neg, cumulative_pos};
    }
}

size_t BorderSystem::dim() const {
    return dim_;
}

size_t BorderSystem::size() const {
    if (pLowerBorder_.size() != pUpperBorder_.size())
        throw std::domain_error("Lower and upper border counts must be the same!");
    return pLowerBorder_.size();
}

const std::shared_ptr<Border> &BorderSystem::get_lower(size_t i) const {
    return pLowerBorder_[i];
}

const std::shared_ptr<Border> &BorderSystem::get_upper(size_t i) const {
    return pUpperBorder_[i];
}

std::pair<double, double> BorderSystem::confidence_interval(const std::vector<double> & v) const {
    const auto cb = containing_borders(v);
    return confidence_interval(cb);
}

double BorderSystem::confidence(const std::vector<double> & v) const {
    const auto cb = containing_borders(v);
    return confidence(cb);
}

std::pair<double, double> BorderSystem::confidence_interval(const std::pair<int, int> & cb) const {
    int l=0, u=0;
    double n=0, p=0;
    double conf_low, conf_up;
    std::tie(l,u) = cb;
    if ( l == -1 ) {
        conf_low = 0;
    } else {
        std::tie(n,p) = pLowerBorder_[l]->get_neg_pos_counts();
        conf_low = p/(n+p);
    }
    if ( (size_t)u == size() ) {
        conf_up = 1;
    } else {
        std::tie(n,p) = pLowerBorder_[u]->get_neg_pos_counts();
        conf_up = p/(n+p);
    }
    return {conf_low, conf_up};
}

double BorderSystem::confidence(const std::pair<int, int> & cb) const {
    const auto size = (int)this->size();
    int l=0, u=0;
    double n=0, p=0;
    std::tie(l,u) = cb;
    if (l != -1)
        std::tie(n,p) = pLowerBorder_[l]->get_neg_pos_counts();
    if (l == u) {
        if (l == -1 || u == size) throw std::runtime_error("unexpected error");
        return p/(n+p);
    }
    double neg_buffer = 0;
    double pos_buffer = 0;
    if (u == size) {
        std::tie(neg_buffer,pos_buffer) = cumulative_neg_pos_[size-1];
    } else {
        std::tie(neg_buffer,pos_buffer) = cumulative_neg_pos_[u];
    }
    if (l != -1) {
        neg_buffer = neg_buffer - cumulative_neg_pos_[l].first + n;
        pos_buffer = pos_buffer - cumulative_neg_pos_[l].second + p;
    }
    return pos_buffer/(neg_buffer+pos_buffer);
}

const BorderSystem &BorderSystem::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const size_t dim = this->dim();
    const size_t size = this->size();
    pt.put("dim", dim);
    pt.put("size", size);
    ptree lowerBorderChildren;
    ptree upperBorderChildren;
    for (size_t i = 0; i < this->size(); ++i) {
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

std::pair<int, int> BorderSystem::containing_borders(const std::vector<double> & v) const {
    const int size = (int)this->size();
    int l=-1, u=size;
    for (int i = 0;    i < size; i++) if (pUpperBorder_[i]->point_below(v)) {u=i;break;}
    for (int i = size-1; i >= 0; i--) if (pLowerBorder_[i]->point_above(v)) {l=i;break;}
    return {l,u};
}

#endif
