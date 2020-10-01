#ifndef LIB_BORDER_SYSTEM_H_
#define LIB_BORDER_SYSTEM_H_

#include "border.h"
#include "layer_partitioning.h"

class BorderSystem {
public:
    explicit BorderSystem(const std::shared_ptr<LayerPartitioning> &);
    explicit BorderSystem(const boost::property_tree::ptree &);

    unsigned int get_dim() const;
    unsigned int get_size() const;
    bool consistent() const;

    const std::shared_ptr<Border> & get_lower(unsigned int) const;
    const std::shared_ptr<Border> & get_upper(unsigned int) const;

    std::pair< int, int > containing_borders(const std::vector<double> &, bool);

    const BorderSystem & dump_to_ptree(boost::property_tree::ptree &) const;

private:
    const unsigned int dim_;
    std::vector<std::shared_ptr<Border>> pLowerBorder_;
    std::vector<std::shared_ptr<Border>> pUpperBorder_;

    std::pair< int, int > containing_borders_slow(const std::vector<double> &);
    std::pair< int, int > containing_borders_fast(const std::vector<double> &);
};

BorderSystem::BorderSystem(const std::shared_ptr<LayerPartitioning> & pLP): dim_(pLP->get_dim()) {
    //todo: implement
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

unsigned int BorderSystem::get_dim() const {
    return dim_;
}

unsigned int BorderSystem::get_size() const {
    if (pLowerBorder_.size() != pUpperBorder_.size())
        throw std::domain_error("Lower and upper border counts must be the same!");
    return pLowerBorder_.size();
}

bool BorderSystem::consistent() const {
    //todo: implement
    return false;
}

const std::shared_ptr<Border> &BorderSystem::get_lower(unsigned int i) const {
    return pLowerBorder_[i];
}

const std::shared_ptr<Border> &BorderSystem::get_upper(unsigned int i) const {
    return pUpperBorder_[i];
}

std::pair<int, int> BorderSystem::containing_borders(const std::vector<double> & v, bool fast = true) {
    if (fast)
        return containing_borders_fast(v);
    return containing_borders_slow(v);
}

const BorderSystem &BorderSystem::dump_to_ptree(boost::property_tree::ptree & pt) const {
    //todo: implement
    return *this;
}

std::pair<int, int> BorderSystem::containing_borders_slow(const std::vector<double> & v) {
    return {0,0};
}

std::pair<int, int> BorderSystem::containing_borders_fast(const std::vector<double> & v) {
    //todo: implement
    return containing_borders_slow(v);
}


#endif