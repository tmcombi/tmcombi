#ifndef LIB_CLASSIFIER_H_
#define LIB_CLASSIFIER_H_

#include <vector>
#include <boost/property_tree/ptree.hpp>

class Classifier {
public:
    [[nodiscard]] virtual size_t dim() const = 0;

    [[nodiscard]] virtual double confidence(const std::vector<double> &) const = 0;
    [[nodiscard]] virtual std::pair<double,double> confidence_interval(const std::vector<double> &) const;
    [[nodiscard]] virtual std::pair<double,std::pair<double,double>>
    confidence_and_confidence_interval(const std::vector<double> &) const;

    virtual void dump_to_ptree(boost::property_tree::ptree &) const = 0;

    virtual ~Classifier() = default;
};

std::pair<double, double> Classifier::confidence_interval(const std::vector<double> & v) const {
    const double conf = this->confidence(v);
    return {conf,conf};
}

std::pair<double, std::pair<double, double>>
Classifier::confidence_and_confidence_interval(const std::vector<double> & v) const {
    const double conf = this->confidence(v);
    return { conf, {conf, conf} };
}


#endif