#ifndef LIB_FEATURE_VECTOR_H_
#define LIB_FEATURE_VECTOR_H_

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>

class FeatureVector {
public:
    explicit FeatureVector(std::vector<double>);
    explicit FeatureVector(const std::string &, const std::vector<unsigned int> &,
            unsigned int, const std::string &, const std::string &, int);
    explicit FeatureVector(const boost::property_tree::ptree &);
    ~FeatureVector();

    unsigned int get_dim() const;
    double get_weight_negatives() const;
    double get_weight_positives() const;
    const std::vector<double> & get_data() const;
    double operator[](unsigned int) const;
    bool operator==(const FeatureVector &) const;
    bool operator!=(const FeatureVector &) const;
    bool operator<=(const FeatureVector &) const;
    bool operator>=(const FeatureVector &) const;
    bool operator<(const FeatureVector &) const;
    bool operator>(const FeatureVector &) const;

    FeatureVector & inc_weight_negatives(double);
    FeatureVector & inc_weight_positives(double);

    const FeatureVector & dump_to_ptree(boost::property_tree::ptree &) const;

private:
    double weight_negatives_{};
    double weight_positives_{};
    std::vector<double> data_;
};

unsigned int FeatureVector::get_dim() const {
    return data_.size();
}

double FeatureVector::get_weight_negatives() const {
    return weight_negatives_;
}

double FeatureVector::get_weight_positives() const {
    return weight_positives_;
}

FeatureVector & FeatureVector::inc_weight_negatives(const double d) {
    weight_negatives_ += d;
    return *this;
}

FeatureVector & FeatureVector::inc_weight_positives(const double d) {
    weight_positives_ += d;
    return *this;
}

FeatureVector::FeatureVector(std::vector<double> data):
weight_negatives_ {0},
weight_positives_ {0},
data_ {std::move(data)}
{}

double FeatureVector::operator[](const unsigned int i) const {
    if (i>=data_.size()) throw std::out_of_range("Index must not exceed the size!");
    return data_[i];
}

FeatureVector::FeatureVector(const std::string & data, const std::vector<unsigned int> & selected_feature_index,
                             unsigned int target_feature_index, const std::string & negatives_label,
                             const std::string & positives_label, const int weight_index = -1):
weight_negatives_ {0},
weight_positives_ {0},
data_(selected_feature_index.size())
{
    std::vector<std::string> str_vector;
    boost::split(str_vector, data, boost::is_any_of(","));

    for (int i = 0; i < selected_feature_index.size(); ++i) {
        if (selected_feature_index[i] >= str_vector.size()) throw std::out_of_range("weight_index out of range!");
        data_[i] = std::stod(str_vector[selected_feature_index[i]]);
    }

    int weight = 1;
    if (weight_index >= 0) {
        if (weight_index>=str_vector.size()) throw std::out_of_range("weight_index out of range!");
        weight = std::stoi(str_vector[weight_index]);
        if (weight <= 0) throw std::invalid_argument("weight is expected to be positive!");
    }

    if (target_feature_index>=str_vector.size()) throw std::out_of_range("target_feature_index out of range!");
    if (str_vector[target_feature_index] == negatives_label)
        weight_negatives_ = weight;
    else if (str_vector[target_feature_index] == positives_label)
        weight_positives_ = weight;
    else
        throw std::invalid_argument("Class label is unknown!");
}

bool FeatureVector::operator==(const FeatureVector & fv) const {
    return data_ == fv.data_;
}

bool FeatureVector::operator!=(const FeatureVector & fv) const {
    return data_ != fv.data_;
}

bool FeatureVector::operator<=(const FeatureVector & fv) const {
    for (unsigned int i=0; i<data_.size(); ++i)
        if (data_[i] > fv.data_[i]) return false;
    return true;
}

bool FeatureVector::operator>=(const FeatureVector & fv) const {
    for (unsigned int i=0; i<data_.size(); ++i)
        if (data_[i] < fv.data_[i]) return false;
    return true;
}

bool FeatureVector::operator<(const FeatureVector & fv) const {
    return *this <= fv & *this != fv;
}

bool FeatureVector::operator>(const FeatureVector & fv) const {
    return *this >= fv & *this != fv;
}

const std::vector<double> &FeatureVector::get_data() const {
    return data_;
}

const FeatureVector & FeatureVector::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    const unsigned int dim = get_dim();
    pt.put("dim", dim);
    ptree children;
    for (unsigned int i=0; i < dim; ++i) {
        ptree child;
        child.put("", data_.at(i));
        children.push_back(std::make_pair("", child));
    }
    pt.add_child("data", children);
    pt.put("w_neg", get_weight_negatives());
    pt.put("w_pos", get_weight_positives());
    return *this;
}

FeatureVector::FeatureVector(const boost::property_tree::ptree & pt) {
    const unsigned int dim = pt.get<double>("dim");
    for (auto& item : pt.get_child("data"))
        data_.push_back(item.second.get_value<double>());
    if (dim != get_dim())
        throw std::domain_error("Error during parsing of json-ptree: dim does not correspond to the vector dim!");
    weight_negatives_ = pt.get<double>("w_neg");
    weight_positives_ = pt.get<double>("w_pos");
}

FeatureVector::~FeatureVector() = default;

std::ostream & operator<<(std::ostream & stream, const FeatureVector & fv) {
    stream << "[data:{";
    if (!fv.get_data().empty()) {
        stream << fv[0];
    }
    for (unsigned int i = 1; i < fv.get_dim(); ++i) {
        stream << ',' << fv[i];
    }
    stream << "},w_neg:" << fv.get_weight_negatives() << ",w_pos:" << fv.get_weight_positives() << ']';
    return stream;
}

#endif
