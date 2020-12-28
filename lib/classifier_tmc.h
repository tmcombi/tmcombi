#ifndef LIB_CLASSIFIER_TMC_H_
#define LIB_CLASSIFIER_TMC_H_

#include <utility>

#include "classifier.h"
#include "border_system.h"

class ClassifierTmc : public Classifier {
public:
    ClassifierTmc() = delete;
    explicit ClassifierTmc(std::shared_ptr<const BorderSystem>);

    double confidence(const std::vector<double> &) const override;
    std::pair<double,double> confidence_interval(const std::vector<double> &) const override;
    std::pair<double,std::pair<double,double>>
    confidence_and_confidence_interval(const std::vector<double> &) const override;

    void dump_to_ptree(boost::property_tree::ptree &) const override;

private:
    const std::shared_ptr<const BorderSystem> pBS_;
};

ClassifierTmc::ClassifierTmc(std::shared_ptr<const BorderSystem> pBS) : pBS_(std::move(pBS)) {
}

double ClassifierTmc::confidence(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return pBS_->confidence(cb);
}

std::pair<double, double> ClassifierTmc::confidence_interval(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return pBS_->confidence_interval(cb);
}

std::pair<double, std::pair<double, double>>
ClassifierTmc::confidence_and_confidence_interval(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return {pBS_->confidence(cb), pBS_->confidence_interval(cb)};
}

void ClassifierTmc::dump_to_ptree(boost::property_tree::ptree & pt) const {
    pt.put("type", "ClassifierTmc");
    pBS_->dump_to_ptree(pt);
}

#endif