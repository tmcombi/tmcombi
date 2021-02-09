#ifndef LIB_CLASSIFIER_WEIGHTED_SUM_H_
#define LIB_CLASSIFIER_WEIGHTED_SUM_H_

#include <utility>

#include "classifier.h"
#include "feature_transform.h"

class ClassifierWeightedSum : public Classifier {
public:
    explicit ClassifierWeightedSum(std::vector<double>, double);
    explicit ClassifierWeightedSum(const boost::property_tree::ptree &);

    size_t dim() const override;
    double confidence(const std::vector<double> &) const override;

    void dump_to_ptree(boost::property_tree::ptree &) const override;

private:
    std::vector<double> weights_;
    double bias_;
};

ClassifierWeightedSum::ClassifierWeightedSum(std::vector<double>  weights, const double bias = 0) :
weights_(std::move(weights)), bias_(bias) {
}

ClassifierWeightedSum::ClassifierWeightedSum(const boost::property_tree::ptree & pt) {
    if ( pt.get<std::string>("type") != "ClassifierWeightedSum" )
        throw std::runtime_error("Expecting configuration of type ClassifierWeightedSum");
    for (auto& item : pt.get_child("weights")) {
        weights_.push_back(item.second.get_value<double>());
    }
    bias_ = pt.get<double>("bias");
}

size_t ClassifierWeightedSum::dim() const {
    return weights_.size();
}

double ClassifierWeightedSum::confidence(const std::vector<double> & v) const {
    double sum = 0;
    const auto dim = this->dim();
    if (v.size() != dim)
        throw std::runtime_error("Expecting input of different dimension");
    for (size_t i = 0; i < dim; i++) {
        sum+=weights_[i]*v[i];
    }
    sum += bias_;
    return sum;
}

void ClassifierWeightedSum::dump_to_ptree(boost::property_tree::ptree & pt) const {
    const auto dim = this->dim();
    using boost::property_tree::ptree;
    pt.put("type", "ClassifierWeightedSum");

    ptree weights;
    for (size_t i = 0; i < dim; i++) {
        ptree weight;
        weight.put("", weights_.at(i));
        weights.push_back(std::make_pair("", weight));
    }

    pt.add_child("weights", weights);
    pt.put("bias", bias_);
}

#endif