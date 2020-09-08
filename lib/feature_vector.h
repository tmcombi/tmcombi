#ifndef LIB_FEATURE_VECTOR_H_
#define LIB_FEATURE_VECTOR_H_

#include <vector>

class FeatureVector {
public:
    explicit FeatureVector(std::vector<double>);
    ~FeatureVector();

    unsigned int get_dim() const;
    double get_weight_negatives() const;
    double get_weight_positives() const;
    double operator[](unsigned int) const;

    FeatureVector & inc_weight_negatives(double);
    FeatureVector & inc_weight_positives(double);

    friend std::ostream& operator<< (std::ostream&, const FeatureVector&);

private:
    double weight_negatives;
    double weight_positives;
    std::vector<double> data;
};

unsigned int FeatureVector::get_dim() const {
    return data.size();
}

double FeatureVector::get_weight_negatives() const {
    return weight_negatives;
}

double FeatureVector::get_weight_positives() const {
    return weight_positives;
}

FeatureVector & FeatureVector::inc_weight_negatives(const double d) {
    weight_negatives += d;
    return *this;
}

FeatureVector & FeatureVector::inc_weight_positives(const double d) {
    weight_positives += d;
    return *this;
}

FeatureVector::FeatureVector(std::vector<double> data):
weight_negatives {0},
weight_positives {0},
data {std::move(data)}
{}

double FeatureVector::operator[](const unsigned int i) const {
    return data[i];
}

std::ostream & operator<<(std::ostream & stream, const FeatureVector & fv) {
    stream << "[data:{";
    if (!fv.data.empty()) {
        stream << fv.data[0];
    }
    for (unsigned int i = 1; i < fv.data.size(); ++i) {
        stream << ',' << fv.data[i];
    }
    stream << "},w_neg:" << fv.weight_negatives << ",w_pos:" << fv.weight_positives << ']';
    return stream;
}

FeatureVector::~FeatureVector() = default;

#endif
