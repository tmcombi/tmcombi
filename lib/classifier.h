#ifndef LIB_CLASSIFIER_H_
#define LIB_CLASSIFIER_H_

class Classifier {
public:
    virtual double confidence(const std::vector<double> &) const = 0;
    virtual std::pair<double,double> confidence_interval(const std::vector<double> &) const;
    virtual std::pair<double,std::pair<double,double>>
    confidence_and_confidence_interval(const std::vector<double> &) const;
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