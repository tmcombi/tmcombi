#ifndef LIB_DATA_CONTAINER_H_
#define LIB_DATA_CONTAINER_H_

#include <vector>
#include "feature_vector.h"

class DataContainer {
public:
    explicit DataContainer(unsigned int); // unsigned int = dimension

    virtual unsigned int push(const std::shared_ptr<FeatureVector>& ) = 0;

    unsigned int dim() const;
    unsigned int size() const;
    virtual const std::pair<double, double> & get_neg_pos_counts() const;
    const std::shared_ptr<FeatureVector> & operator[](unsigned int) const;

    virtual bool contains(const std::shared_ptr<FeatureVector> &) const;

    // has no item greater than any of another DataContainer. Slow -> do not use in production code!
    bool operator<=(const DataContainer &) const;
    // has no item smaller than any of another DataContainer. Slow -> do not use in production code!
    bool operator>=(const DataContainer &) const;

    virtual bool has_no_intersection_with(const DataContainer &) const;

    virtual const DataContainer & dump_to_ptree(boost::property_tree::ptree &) const;

protected:
    std::pair<double, double> total_neg_pos_counts_;
    std::vector<std::shared_ptr<FeatureVector>> pFV_;

private:
    const unsigned int dim_;
};

DataContainer::DataContainer(unsigned int dim) : total_neg_pos_counts_(0,0), dim_(dim) {
}

unsigned int DataContainer::dim() const {
    return dim_;
}

unsigned int DataContainer::size() const {
    return pFV_.size();
}

const std::shared_ptr<FeatureVector>& DataContainer::operator[](unsigned int i) const {
    return pFV_[i];
}

const std::pair<double, double> & DataContainer::get_neg_pos_counts() const {
    return total_neg_pos_counts_;
}

bool DataContainer::contains(const std::shared_ptr<FeatureVector> & pFV) const {
    for (unsigned int i = 0; i < size(); i++)
        if (pFV->get_data() == pFV_[i]->get_data()) return true;
    return false;
}

bool DataContainer::operator<=(const DataContainer & DataContainer) const {
    if (dim() != DataContainer.dim())
        throw std::domain_error("Unexpected error: trying to compare DataContainers of different dimensions!");
    for ( unsigned int i=0; i < size(); ++i )
        for ( unsigned int j=0; j < DataContainer.size(); ++j )
            if ( *this->operator[](i) > *DataContainer[j] ) return false;
    return true;
}

bool DataContainer::operator>=(const DataContainer & DataContainer) const {
    if (dim() != DataContainer.dim())
        throw std::domain_error("Unexpected error: trying to compare DataContainers of different dimensions!");
    for ( unsigned int i=0; i < size(); ++i )
        for ( unsigned int j=0; j < DataContainer.size(); ++j )
            if ( *this->operator[](i) < *DataContainer[j] ) return false;
    return true;
}

bool DataContainer::has_no_intersection_with(const DataContainer & DataContainer) const {
    if (dim() != DataContainer.dim())
        throw std::domain_error("Unexpected error: trying to compare DataContainers of different dimensions!");
    for ( unsigned int i=0; i < size(); ++i )
        if (DataContainer.contains((this->operator[](i))))
            return false;
    return true;
}

const DataContainer &DataContainer::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const unsigned int dim = this->dim();
    const unsigned int size = this->size();
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

std::ostream &operator<<(std::ostream & stream, const DataContainer & dc) {
    if (dc.size()) {
        stream << *dc[0];
    }
    for (unsigned int i = 1; i < dc.size(); ++i) {
        stream << ';' << *dc[i];
    }
    return stream;
}


#endif
