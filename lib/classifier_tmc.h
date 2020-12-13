#ifndef LIB_CLASSIFIER_TMC_H_
#define LIB_CLASSIFIER_TMC_H_

#include <utility>

#include "classifier.h"
#include "border_system.h"

class ClassifierTmc : public Classifier {
public:
    ClassifierTmc() = delete;
    explicit ClassifierTmc(std::shared_ptr<const BorderSystem>);
    explicit ClassifierTmc(const boost::property_tree::ptree &);

    label get_label(const std::vector<double> &) const override;
    double confidence(const std::vector<double> &) const override;
    std::pair<double,double> confidence_interval(const std::vector<double> &) const override;

    std::pair<label,double> label_and_confidence(const std::vector<double> &) const override;
    std::pair<label,std::pair<double,double>> label_and_confidence_interval(const std::vector<double> &) const override;
    std::pair<double,std::pair<double,double>> confidence_and_confidence_interval(const std::vector<double> &) const override;

    std::pair<std::pair<label,double>,std::pair<double,double>>
    label_and_confidence_and_confidence_interval(const std::vector<double> &) const override;

private:
    label get_label(const std::pair<int, int> &) const; // from containing borders

    const std::shared_ptr<const BorderSystem> pBS_;
};

ClassifierTmc::ClassifierTmc(std::shared_ptr<const BorderSystem> pBS) : pBS_(std::move(pBS)) {
}

ClassifierTmc::ClassifierTmc(const boost::property_tree::ptree & pt) : pBS_(std::make_shared<BorderSystem>(pt)) {
    if( pt.get<std::string>("type") != "classifier_tmc")
        throw std::runtime_error("Configuration for ClassifierTmc should be of type = \"classifier_tmc\"");
}

// from containing borders
Classifier::label ClassifierTmc::get_label(const std::pair<int, int> & cb) const {
    double a = 0, b = 0;
    std::tie(a,b) = pBS_->confidence_interval(cb);
    if (b < 0.5) return Classifier::negative;
    if (a > 0.5) return Classifier::positive;
    return Classifier::undefined;
}

Classifier::label ClassifierTmc::get_label(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return get_label(cb);
}

double ClassifierTmc::confidence(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return pBS_->confidence(cb);
}

std::pair<double, double> ClassifierTmc::confidence_interval(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return pBS_->confidence_interval(cb);
}

std::pair<Classifier::label, double> ClassifierTmc::label_and_confidence(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return {get_label(cb), pBS_->confidence(cb)};
}

std::pair<Classifier::label, std::pair<double, double>>
ClassifierTmc::label_and_confidence_interval(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return {get_label(cb), pBS_->confidence_interval(cb)};
}

std::pair<double, std::pair<double, double>>
ClassifierTmc::confidence_and_confidence_interval(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return {pBS_->confidence(cb), pBS_->confidence_interval(cb)};
}

std::pair<std::pair<Classifier::label, double>, std::pair<double, double>>
ClassifierTmc::label_and_confidence_and_confidence_interval(const std::vector<double> & v) const {
    const auto cb = pBS_->containing_borders(v);
    return { {get_label(cb), pBS_->confidence(cb)}, pBS_->confidence_interval(cb) };
}

#endif