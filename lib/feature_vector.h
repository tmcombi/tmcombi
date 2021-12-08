#ifndef LIB_FEATURE_VECTOR_H_
#define LIB_FEATURE_VECTOR_H_

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include "int_type.h"

template <typename WeightType = double>
class FeatureVectorTemplated {
public:
    explicit FeatureVectorTemplated(size_t);
    explicit FeatureVectorTemplated(std::vector<double>);
    FeatureVectorTemplated(const std::string &, const std::vector<size_t> &,
            size_t, const std::string &, const std::string &, int weight_index = -1);
    explicit FeatureVectorTemplated(const boost::property_tree::ptree &);

    ~FeatureVectorTemplated();

    [[nodiscard]] size_t dim() const;
    [[nodiscard]] WeightType get_weight_negatives() const;
    [[nodiscard]] WeightType get_weight_positives() const;
    [[nodiscard]] const std::vector<double> & get_data() const;
    double operator[](size_t) const;
    bool operator==(const FeatureVectorTemplated &) const;
    bool operator!=(const FeatureVectorTemplated &) const;

    bool operator<=(const std::vector<double> &) const;
    bool operator>=(const std::vector<double> &) const;
    bool operator<(const std::vector<double> &) const;
    bool operator>(const std::vector<double> &) const;

    bool operator<=(const FeatureVectorTemplated &) const;
    bool operator>=(const FeatureVectorTemplated &) const;
    bool operator<(const FeatureVectorTemplated &) const;
    bool operator>(const FeatureVectorTemplated &) const;

    FeatureVectorTemplated & inc_weight_negatives(WeightType);
    FeatureVectorTemplated & inc_weight_positives(WeightType);

    FeatureVectorTemplated & set_data(size_t,double);

    const FeatureVectorTemplated & dump_to_ptree(boost::property_tree::ptree &) const;

private:
    WeightType weight_negatives_{};
    WeightType weight_positives_{};
    std::vector<double> data_;

    friend class FeatureTransform;
};

template<typename WeightType>
size_t FeatureVectorTemplated<WeightType>::dim() const {
    return data_.size();
}

template<typename WeightType>
WeightType FeatureVectorTemplated<WeightType>::get_weight_negatives() const {
    return weight_negatives_;
}

template<typename WeightType>
WeightType FeatureVectorTemplated<WeightType>::get_weight_positives() const {
    return weight_positives_;
}

template<typename WeightType>
FeatureVectorTemplated<WeightType> & FeatureVectorTemplated<WeightType>::inc_weight_negatives(const WeightType d) {
    weight_negatives_ += d;
    return *this;
}

template<typename WeightType>
FeatureVectorTemplated<WeightType> & FeatureVectorTemplated<WeightType>::inc_weight_positives(const WeightType d) {
    weight_positives_ += d;
    return *this;
}

template<typename WeightType>
FeatureVectorTemplated<WeightType> & FeatureVectorTemplated<WeightType>::set_data(const size_t n, const double d) {
    data_[n] = d;
    return *this;
}

template<typename WeightType>
FeatureVectorTemplated<WeightType>::FeatureVectorTemplated(size_t dim) :
weight_negatives_ {0},
weight_positives_ {0},
data_(dim) {
}

template<typename WeightType>
FeatureVectorTemplated<WeightType>::FeatureVectorTemplated(std::vector<double> data):
weight_negatives_ {0},
weight_positives_ {0},
data_ {std::move(data)}
{}

template<typename WeightType>
double FeatureVectorTemplated<WeightType>::operator[](const size_t i) const {
    //if (i>=data_.size()) throw std::out_of_range("Index must not exceed the size!");
    return data_[i];
}

template<typename WeightType>
FeatureVectorTemplated<WeightType>::
FeatureVectorTemplated(const std::string & data, const std::vector<size_t> & selected_feature_index,
                       size_t target_feature_index, const std::string & negatives_label,
                       const std::string & positives_label, const int weight_index):
weight_negatives_ {0},
weight_positives_ {0},
data_(selected_feature_index.size())
{
    std::vector<std::string> str_vector;
    boost::split(str_vector, data, boost::is_any_of(","));

    for (size_t i = 0; i < selected_feature_index.size(); ++i) {
        if (selected_feature_index[i] >= str_vector.size())
            throw std::out_of_range("weight_index out of range!");
        const auto & value_str = str_vector[selected_feature_index[i]];
        if (value_str == "?")
            throw std::domain_error("value for a feature is set to '?', i.e. not set at all. Not accepting such values a.t.m.");
        data_[i] = std::stod(value_str);
    }

    WeightType weight = 1;
    if (weight_index >= 0) {
        if ((size_t)weight_index>=str_vector.size()) throw std::out_of_range("weight_index out of range!");
        weight = std::stod(str_vector[weight_index]);
        if (weight <= 0)
            throw std::invalid_argument("weight is expected to be positive, but got "
            + str_vector[weight_index] + " -> " + std::to_string(weight));
    }

    if (target_feature_index>=str_vector.size()) throw std::out_of_range("target_feature_index out of range!");
    if (str_vector[target_feature_index] == negatives_label)
        weight_negatives_ = weight;
    else if (str_vector[target_feature_index] == positives_label)
        weight_positives_ = weight;
    else
        throw std::invalid_argument("Class label is unknown!");
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator==(const FeatureVectorTemplated<WeightType> & fv) const {
    return data_ == fv.data_;
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator!=(const FeatureVectorTemplated<WeightType> & fv) const {
    return data_ != fv.data_;
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator<=(const std::vector<double> & v) const {
    const size_t dim = data_.size();
    if (dim != v.size())
        throw std::domain_error("Expecting data of the same dimension");
    for (size_t i=0; i<dim; ++i)
        if (data_[i] > v[i]) return false;
    return true;
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator<=(const FeatureVectorTemplated<WeightType> & fv) const {
    return operator<=(fv.data_);
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator>=(const std::vector<double> & v) const {
    const size_t dim = data_.size();
    if (dim != v.size())
        throw std::domain_error("Expecting data of the same dimension");
    for (size_t i=0; i<dim; ++i)
        if (data_[i] < v[i]) return false;
    return true;
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator>=(const FeatureVectorTemplated<WeightType> & fv) const {
    return operator>=(fv.data_);
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator<(const std::vector<double> & v) const {
    return operator<=(v) && (data_ != v);
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator<(const FeatureVectorTemplated<WeightType> & fv) const {
    return operator<(fv.data_);
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator>(const std::vector<double> & v) const {
    return operator>=(v) && (data_ != v);
}

template<typename WeightType>
bool FeatureVectorTemplated<WeightType>::operator>(const FeatureVectorTemplated<WeightType> & fv) const {
    return operator>(fv.data_);
}

template<typename WeightType>
const std::vector<double> &FeatureVectorTemplated<WeightType>::get_data() const {
    return data_;
}

template<typename WeightType>
const FeatureVectorTemplated<WeightType> &
        FeatureVectorTemplated<WeightType>::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    pt.put("type", "FeatureVector");
    const size_t dim = this->dim();
    pt.put("dim", dim);
    ptree children;
    for (size_t i=0; i < dim; ++i) {
        ptree child;
        child.put("", data_.at(i));
        children.push_back(std::make_pair("", child));
    }
    pt.add_child("data", children);
    pt.put("w_neg", get_weight_negatives());
    pt.put("w_pos", get_weight_positives());
    return *this;
}

template<typename WeightType>
FeatureVectorTemplated<WeightType>::FeatureVectorTemplated(const boost::property_tree::ptree & pt) {
    if(pt.get<std::string>("type") != "FeatureVector") {
        throw std::runtime_error("unexpected error");
    }
    const auto dim = pt.get<size_t>("dim");
    for (auto& item : pt.get_child("data"))
        data_.push_back(item.second.get_value<double>());
    if (dim != this->dim())
        throw std::domain_error("Error during parsing of json-ptree: dim does not correspond to the vector dim!");
    weight_negatives_ = pt.get<WeightType>("w_neg");
    weight_positives_ = pt.get<WeightType>("w_pos");
}

template<typename WeightType>
FeatureVectorTemplated<WeightType>::~FeatureVectorTemplated() = default;

template<typename WeightType>
std::ostream & operator<<(std::ostream & stream, const FeatureVectorTemplated<WeightType> & fv) {
    stream << "[data:{";
    if (!fv.get_data().empty()) {
        stream << fv[0];
    }
    for (size_t i = 1; i < fv.dim(); ++i) {
        stream << ',' << fv[i];
    }
    stream << "},w_neg:" << fv.get_weight_negatives() << ",w_pos:" << fv.get_weight_positives() << ']';
    return stream;
}

typedef FeatureVectorTemplated<IntType> FeatureVectorInt;
typedef FeatureVectorTemplated<double> FeatureVectorDouble;
typedef FeatureVectorTemplated<> FeatureVector;


#endif
