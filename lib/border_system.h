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

BorderSystem::BorderSystem(const std::shared_ptr<LayerPartitioning> & pLP):
dim_(pLP->get_dim()), pLowerBorder_(pLP->get_size()), pUpperBorder_(pLP->get_size())  {
    const SampleCreator sample_creator;

    unsigned int counter = 0;
    std::shared_ptr<Border> pCurrentUpper = std::make_shared<Border>(dim_);
    for (const auto & it : *pLP) {
        const std::shared_ptr<Border> pLayerUpperPart = sample_creator.upper_border(it);
        const std::shared_ptr<Sample> pCurrentUpperMergedWithLayerUpperPart =
                sample_creator.merge(pCurrentUpper,pLayerUpperPart);
        pCurrentUpper = sample_creator.upper_border(pCurrentUpperMergedWithLayerUpperPart);
        pCurrentUpper->set_neg_pos_counts(it->get_neg_pos_counts());
        pUpperBorder_[counter++] = pCurrentUpper;
    }

    std::shared_ptr<Border> pCurrentLower = std::make_shared<Border>(dim_);
    for (auto it = pLP->rbegin(); it != pLP->rend(); ++it) {
        const std::shared_ptr<Border> pLayerLowerPart = sample_creator.lower_border(*it);
        const std::shared_ptr<Sample> pCurrentLowerMergedWithLayerLowerPart =
                sample_creator.merge(pCurrentLower,pLayerLowerPart);
        pCurrentLower = sample_creator.lower_border(pCurrentLowerMergedWithLayerLowerPart);
        pCurrentLower->set_neg_pos_counts((*it)->get_neg_pos_counts());
        pLowerBorder_[--counter] = pCurrentLower;
    }
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
    for (unsigned int i = 0; i < get_size(); ++i) {
        if (!pLowerBorder_[i]->consistent()) return false;
        if (!pUpperBorder_[i]->consistent()) return false;
    }
    for (unsigned int i = 0; i < get_size()-1 ; ++i) {
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
    if (fast)
        return containing_borders_fast(v);
    return containing_borders_slow(v);
}

const BorderSystem &BorderSystem::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const unsigned int dim = get_dim();
    const unsigned int size = get_size();
    pt.put("dim", dim);
    pt.put("size", size);
    ptree lowerBorderChildren;
    ptree upperBorderChildren;
    for (unsigned int i = 0; i < get_size(); ++i) {
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
    return {0,0};
}

std::pair<int, int> BorderSystem::containing_borders_fast(const std::vector<double> & v) {
    //todo: implement
    return containing_borders_slow(v);
}


#endif
