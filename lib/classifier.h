#ifndef LIB_CLASSIFIER_H_
#define LIB_CLASSIFIER_H_

class Classifier {
public:
    enum label {negative, positive, undefined};

    virtual label get_label(const std::vector<double> &) const;
    virtual double confidence(const std::vector<double> &) const = 0;
    virtual std::pair<double,double> confidence_interval(const std::vector<double> &) const;

    virtual std::pair<label,double> label_and_confidence(const std::vector<double> &) const;
    virtual std::pair<label,std::pair<double,double>> label_and_confidence_interval(const std::vector<double> &) const;
    virtual std::pair<double,std::pair<double,double>> confidence_and_confidence_interval(const std::vector<double> &) const;

    virtual std::pair<std::pair<label,double>,std::pair<double,double>>
    label_and_confidence_and_confidence_interval(const std::vector<double> &) const;

private:
    static inline label get_label(double);
};

Classifier::label Classifier::get_label(double conf) {
    if (conf < 0.5) return Classifier::negative;
    if (conf > 0.5) return Classifier::positive;
    return Classifier::undefined;
}

Classifier::label Classifier::get_label(const std::vector<double> & v) const {
    const double conf = this->confidence(v);
    return get_label(conf);
}

std::pair<double, double> Classifier::confidence_interval(const std::vector<double> & v) const {
    const double conf = this->confidence(v);
    return {conf,conf};
}

std::pair<Classifier::label, double> Classifier::label_and_confidence(const std::vector<double> & v) const {
    const double conf = this->confidence(v);
    return {get_label(conf), conf};
}

std::pair<Classifier::label, std::pair<double, double>>
Classifier::label_and_confidence_interval(const std::vector<double> & v) const {
    const double conf = this->confidence(v);
    return { get_label(conf), {conf, conf} };
}

std::pair<double, std::pair<double, double>>
Classifier::confidence_and_confidence_interval(const std::vector<double> & v) const {
    const double conf = this->confidence(v);
    return { conf, {conf, conf} };
}

std::pair<std::pair<Classifier::label, double>, std::pair<double, double>>
Classifier::label_and_confidence_and_confidence_interval(const std::vector<double> & v) const {
    const double conf = this->confidence(v);
    return { {get_label(conf), conf}, {conf, conf} };
}


#endif