#ifndef LIB_DATA_CONTAINER_H_
#define LIB_DATA_CONTAINER_H_

#include "feature_vector.h"

template <typename WT>
class DataContainer {
public:
    typedef WT WeightType;

    explicit DataContainer(size_t); // size_t = dimension

#ifdef MEMORY_ANALYSIS
    virtual ~DataContainer();
#else
    virtual ~DataContainer() = default;
#endif

    virtual void push(const std::shared_ptr<FeatureVectorTemplated<WeightType>>& );

    [[nodiscard]] size_t dim() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] virtual const std::pair<WeightType, WeightType> & get_neg_pos_counts() const;
    const std::shared_ptr<FeatureVectorTemplated<WeightType>> & operator[](size_t) const;

    [[nodiscard]] virtual bool contains(const std::shared_ptr<FeatureVectorTemplated<WeightType>> &) const;

    // has no item greater than any of another DataContainer. Slow -> do not use in production code!
    bool operator<=(const DataContainer &) const;
    // has no item smaller than any of another DataContainer. Slow -> do not use in production code!
    bool operator>=(const DataContainer &) const;

    [[nodiscard]] virtual bool has_no_intersection_with(const DataContainer &) const;

    virtual const DataContainer & dump_to_ptree(boost::property_tree::ptree &) const;

protected:
    std::pair<WeightType, WeightType> total_neg_pos_counts_;
    std::vector<std::shared_ptr<FeatureVectorTemplated<WeightType>>> pFV_;

private:
    const size_t dim_;
#ifdef MEMORY_ANALYSIS
    size_t id_;
    static size_t last_id_;
#endif
};

#ifdef MEMORY_ANALYSIS
template <typename WeightType>
size_t DataContainer<WeightType>::last_id_ = 0;
#endif

template<typename WeightType>
DataContainer<WeightType>::DataContainer(size_t dim) : total_neg_pos_counts_(0,0), dim_(dim)
#ifdef MEMORY_ANALYSIS
, id_(last_id_++)
#endif
{
#ifdef MEMORY_ANALYSIS
    std::cout << "DataContainer created " << id_ << std::endl;
#endif
}

template<typename WeightType>
void DataContainer<WeightType>::push(const std::shared_ptr<FeatureVectorTemplated<WeightType>> & pFV) {
    const auto & neg = pFV->get_weight_negatives();
    const auto & pos = pFV->get_weight_positives();
    pFV_.push_back(pFV);
    total_neg_pos_counts_.first += neg;
    total_neg_pos_counts_.second += pos;
}

template<typename WeightType>
size_t DataContainer<WeightType>::dim() const {
    return dim_;
}

template<typename WeightType>
size_t DataContainer<WeightType>::size() const {
    return pFV_.size();
}

template<typename WeightType>
const std::shared_ptr<FeatureVectorTemplated<WeightType>>& DataContainer<WeightType>::operator[](size_t i) const {
    return pFV_[i];
}

template<typename WeightType>
const std::pair<WeightType, WeightType> & DataContainer<WeightType>::get_neg_pos_counts() const {
    return total_neg_pos_counts_;
}

template<typename WeightType>
bool DataContainer<WeightType>::contains(const std::shared_ptr<FeatureVectorTemplated<WeightType>> & pFV) const {
    for (size_t i = 0; i < size(); i++)
        if (pFV->get_data() == pFV_[i]->get_data()) return true;
    return false;
}

template<typename WeightType>
bool DataContainer<WeightType>::operator<=(const DataContainer<WeightType> & DataContainer) const {
    if (dim() != DataContainer.dim())
        throw std::domain_error("Unexpected error: trying to compare DataContainers of different dimensions!");
    for ( size_t i=0; i < size(); ++i )
        for ( size_t j=0; j < DataContainer.size(); ++j )
            if ( *this->operator[](i) > *DataContainer[j] ) return false;
    return true;
}

template<typename WeightType>
bool DataContainer<WeightType>::operator>=(const DataContainer<WeightType> & DataContainer) const {
    if (dim() != DataContainer.dim())
        throw std::domain_error("Unexpected error: trying to compare DataContainers of different dimensions!");
    for ( size_t i=0; i < size(); ++i )
        for ( size_t j=0; j < DataContainer.size(); ++j )
            if ( *this->operator[](i) < *DataContainer[j] ) return false;
    return true;
}

template<typename WeightType>
bool DataContainer<WeightType>::has_no_intersection_with(const DataContainer<WeightType> & DataContainer) const {
    if (dim() != DataContainer.dim())
        throw std::domain_error("Unexpected error: trying to compare DataContainers of different dimensions!");
    for ( size_t i=0; i < size(); ++i )
        if (DataContainer.contains((this->operator[](i))))
            return false;
    return true;
}

template<typename WeightType>
const DataContainer<WeightType> &DataContainer<WeightType>::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const size_t dim = this->dim();
    const size_t size = this->size();
    pt.put("dim", dim);
    pt.put("size", size);
    ptree children;
    for (size_t i = 0; i < size; ++i) {
        ptree child;
        operator[](i)->dump_to_ptree(child);
        children.push_back(std::make_pair("", child));
    }
    pt.add_child("feature_vectors", children);
    pt.put("total_neg", get_neg_pos_counts().first);
    pt.put("total_pos", get_neg_pos_counts().second);
    return *this;
}

#ifdef MEMORY_ANALYSIS
template<typename WT>
DataContainer<WT>::~DataContainer() {
    std::cout << "DataContainer deleted " << id_ << std::endl;
}
#endif

template<typename WeightType>
std::ostream &operator<<(std::ostream & stream, const DataContainer<WeightType> & dc) {
    if (dc.size()) {
        stream << *dc[0];
    }
    for (size_t i = 1; i < dc.size(); ++i) {
        stream << ';' << *dc[i];
    }
    return stream;
}

#endif
