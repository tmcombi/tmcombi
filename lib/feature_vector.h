#ifndef LIB_FEATURE_VECTOR_H_
#define LIB_FEATURE_VECTOR_H_

#include <vector>
#include <boost/algorithm/string.hpp>

class FeatureVector {
public:
    explicit FeatureVector(std::vector<double>);
    explicit FeatureVector(const std::string &, const std::vector<unsigned int> &,
            unsigned int, const std::string &, const std::string &, int);
    ~FeatureVector();

    unsigned int get_dim() const;
    double get_weight_negatives() const;
    double get_weight_positives() const;
    double operator[](unsigned int) const;

    FeatureVector & inc_weight_negatives(double);
    FeatureVector & inc_weight_positives(double);

    friend std::ostream& operator<< (std::ostream&, const FeatureVector&);

private:
    double weight_negatives_;
    double weight_positives_;
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

std::ostream & operator<<(std::ostream & stream, const FeatureVector & fv) {
    stream << "[data:{";
    if (!fv.data_.empty()) {
        stream << fv.data_[0];
    }
    for (unsigned int i = 1; i < fv.data_.size(); ++i) {
        stream << ',' << fv.data_[i];
    }
    stream << "},w_neg:" << fv.weight_negatives_ << ",w_pos:" << fv.weight_positives_ << ']';
    return stream;
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
        data_[i] = std::stod(str_vector[selected_feature_index[i]]);
    }

    unsigned int weight = 1;
    if (weight_index >= 0) {
        weight = std::stoi(str_vector[weight_index]);
    }

    if (str_vector[target_feature_index] == negatives_label)
        weight_negatives_ = weight;
    else if (str_vector[target_feature_index] == positives_label)
        weight_positives_ = weight;
    else
        throw std::invalid_argument("Class label is unknown!");
}


FeatureVector::~FeatureVector() = default;

#endif
