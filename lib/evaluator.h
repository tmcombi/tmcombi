#ifndef LIB_EVALUATOR_H_
#define LIB_EVALUATOR_H_

#include <ostream>

#include "feature_names.h"
#include "sample.h"
#include "border_system.h"

//#define TRACE_EVALUATOR

class Evaluator {
public:
    Evaluator();

    Evaluator & set_sample(const std::shared_ptr<Sample> &);
    Evaluator & set_border_system(const std::shared_ptr<BorderSystem> &);

    /// { {TP,FP}, {FN,TN} }
    std::pair<std::pair<double, double>, std::pair<double, double>> get_confusion_matrix();
    double get_error_rate(); /// 1 - accuracy
    double get_accuracy();
    double get_ranking_conflicts();
    double get_roc_error(); /// 1 - roc
    double get_roc();

    std::ostream & dump_results(std::ostream &);
    std::ostream & evaluate_data_file(std::ostream &, const std::string &, const std::shared_ptr<FeatureNames> &) const;

private:
    std::shared_ptr<Sample> pSample_;
    std::shared_ptr<BorderSystem> pBorderSystem_;

    bool confusion_matrix_computed_;
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_;

    bool ranking_conflicts_computed_;
    double ranking_conflicts_;

    bool confidence_intervals_computed_;

    std::map<double, std::pair<double, double> > confidence2negpos_map_;
    std::map<std::pair<double, double>, std::pair<double, double> > conf_interval2negpos_map_;

    void compute_confidence_intervals();
    void compute_confusion_matrix();
    void compute_ranking_conflicts();
};

Evaluator::Evaluator() : pSample_(nullptr), pBorderSystem_(nullptr),
                         confusion_matrix_computed_(false), confusion_matrix_({{0,0},{0,0}}),
                         ranking_conflicts_computed_(false), ranking_conflicts_(0),
                         confidence_intervals_computed_(false)
{}

Evaluator &Evaluator::set_sample(const std::shared_ptr<Sample> & pSample) {
    pSample_ = pSample;
    confusion_matrix_computed_ = false;
    ranking_conflicts_computed_ = false;
    confidence_intervals_computed_ = false;
    confusion_matrix_ = std::pair<std::pair<double, double>, std::pair<double, double>> ({{0,0},{0,0}});
    ranking_conflicts_ = 0;
    confidence2negpos_map_.clear();
    conf_interval2negpos_map_.clear();
    return *this;
}

Evaluator &Evaluator::set_border_system(const std::shared_ptr<BorderSystem> & pBorderSystem) {
    pBorderSystem_ = pBorderSystem;
    confusion_matrix_computed_ = false;
    ranking_conflicts_computed_ = false;
    confidence_intervals_computed_ = false;
    confusion_matrix_ = std::pair<std::pair<double, double>, std::pair<double, double>> ({{0,0},{0,0}});
    ranking_conflicts_ = 0;
    confidence2negpos_map_.clear();
    conf_interval2negpos_map_.clear();
    return *this;
}

void Evaluator::compute_confidence_intervals() {
#ifdef TIMERS
    const std::clock_t time1 = std::clock();
#endif
    const unsigned int size = pSample_->size();
    for ( unsigned int i = 0; i < size; i++ ) {
        const auto & data = (*pSample_)[i]->get_data();
        const auto confidence_interval = pBorderSystem_->confidence_interval(data);
        /*
#ifdef TRACE_EVALUATOR
        std::cout << "## TRACE_EVALUATOR: feature vector =" << (*pSample_)[i] << std::endl;
        std::cout << "## TRACE_EVALUATOR: confidence interval = (" << confidence_interval.first
                  << ", " << confidence_interval.second << ")" << std::endl;
#endif
         */
        if (confidence_interval.first == confidence_interval.second) {
            const double conf = confidence_interval.first;
            auto it = confidence2negpos_map_.find(conf);
            if ( it == confidence2negpos_map_.end() ) {
                confidence2negpos_map_[conf] = {(*pSample_)[i]->get_weight_negatives(),(*pSample_)[i]->get_weight_positives()};
            } else {
                confidence2negpos_map_[conf].first += (*pSample_)[i]->get_weight_negatives();
                confidence2negpos_map_[conf].second += (*pSample_)[i]->get_weight_positives();
            }
        } else {
            const auto it = conf_interval2negpos_map_.find(confidence_interval);
            if ( it == conf_interval2negpos_map_.end() ) {
                conf_interval2negpos_map_[confidence_interval] = {(*pSample_)[i]->get_weight_negatives(),(*pSample_)[i]->get_weight_positives()};
            } else {
                conf_interval2negpos_map_[confidence_interval].first += (*pSample_)[i]->get_weight_negatives();
                conf_interval2negpos_map_[confidence_interval].second += (*pSample_)[i]->get_weight_positives();
            }
        }
    }
#ifdef TRACE_EVALUATOR
    std::cout << "## TRACE_EVALUATOR: ### START ###" << std::endl;
     for ( auto it = confidence2negpos_map_.begin(); it != confidence2negpos_map_.end(); ++it) {
         const double a = it->first;
         const double n = it->second.first;
         const double p = it->second.second;
         std::cout << "## TRACE_EVALUATOR: confidence = " << a;
         std::cout << "; (neg, pos) = (" << n << ", " << p << ")" << std::endl;
     }
    for ( auto it = conf_interval2negpos_map_.begin(); it != conf_interval2negpos_map_.end(); ++it) {
        const double a = it->first.first;
        const double b = it->first.second;
        const double n = it->second.first;
        const double p = it->second.second;
        std::cout << "## TRACE_EVALUATOR: confidence interval = (" << a << ", " << b << ")";
        std::cout << "; (neg, pos) = (" << n << ", " << p << ")" << std::endl;
    }
    std::cout << "## TRACE_EVALUATOR: ### FINISH ###" << std::endl;
#endif
    confidence_intervals_computed_ = true;
#ifdef TIMERS
    const std::clock_t time2 = std::clock();
    std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Compute confidence intervals" << std::endl;
#endif
}

void Evaluator::compute_confusion_matrix() {
    if (!confidence_intervals_computed_) compute_confidence_intervals();
#ifdef TIMERS
    const std::clock_t time1 = std::clock();
#endif
    confusion_matrix_.first.first = confusion_matrix_.first.second =
    confusion_matrix_.second.first = confusion_matrix_.second.second = 0;
    for ( auto it = confidence2negpos_map_.begin(); it != confidence2negpos_map_.end(); ++it) {
        const double a = it->first;
        const double n = it->second.first; const double p = it->second.second;
        if (a > 0.5) {
            confusion_matrix_.first.first += p;
            confusion_matrix_.first.second += n;
        } else if (a < 0.5) {
            confusion_matrix_.second.first += p;
            confusion_matrix_.second.second += n;
        } else {
            confusion_matrix_.first.first += p/2;
            confusion_matrix_.first.second += n/2;
            confusion_matrix_.second.first += p/2;
            confusion_matrix_.second.second += n/2;
        }

    }
    for ( auto it = conf_interval2negpos_map_.begin(); it != conf_interval2negpos_map_.end(); ++it) {
        const double a = it->first.first; const double b = it->first.second;
        const double n = it->second.first; const double p = it->second.second;
        if (a > 0.5) {
            confusion_matrix_.first.first += p;
            confusion_matrix_.first.second += n;
        } else if (b < 0.5) {
            confusion_matrix_.second.first += p;
            confusion_matrix_.second.second += n;
        } else {
            confusion_matrix_.first.first += p/2;
            confusion_matrix_.first.second += n/2;
            confusion_matrix_.second.first += p/2;
            confusion_matrix_.second.second += n/2;
        }
    }
    confusion_matrix_computed_ = true;
#ifdef TIMERS
    const std::clock_t time2 = std::clock();
    std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Compute confusion matrix" << std::endl;
#endif
}

void Evaluator::compute_ranking_conflicts() {
    if (!confidence_intervals_computed_) compute_confidence_intervals();
#ifdef TIMERS
    const std::clock_t time1 = std::clock();
#endif
    ranking_conflicts_ = 0;
    double current_neg_count=0, current_pos_count=0;
    for ( auto it = confidence2negpos_map_.begin(); it != confidence2negpos_map_.end(); ++it) {
        const double n = it->second.first; const double p = it->second.second;
        ranking_conflicts_ += n * p / 2;
        ranking_conflicts_ += n * current_pos_count;
        current_neg_count += n;
        current_pos_count += p;
    }
    for ( auto it1 = conf_interval2negpos_map_.begin(); it1 != conf_interval2negpos_map_.end(); ++it1) {
        const double a1 = it1->first.first; const double b1 = it1->first.second;
        const double n1 = it1->second.first; const double p1 = it1->second.second;
        ranking_conflicts_ += n1 * p1 / 2;
        auto it2 = it1; ++it2;
        for ( ; it2 != conf_interval2negpos_map_.end(); ++it2) {
            const double a2 = it2->first.first; const double b2 = it2->first.second;
            const double n2 = it2->second.first; const double p2 = it2->second.second;
            if (a2 > b1) {
                ranking_conflicts_ += n2 * p1;
            } else if (a1 > b2) {
                ranking_conflicts_ += n1 * p2;
            } else {
                ranking_conflicts_ += (n2 * p1 + n1 * p2)/2;
            }
        }
        for ( auto it = confidence2negpos_map_.begin(); it != confidence2negpos_map_.end(); ++it) {
            const double a = it->first;
            const double n = it->second.first; const double p = it->second.second;
            if (a > b1) {
                ranking_conflicts_ += n * p1;
            } else if (a1 > a) {
                ranking_conflicts_ += n1 * p;
            } else {
                ranking_conflicts_ += (n * p1 + n1 * p)/2;
            }
        }
    }
    ranking_conflicts_computed_ = true;
#ifdef TIMERS
    const std::clock_t time2 = std::clock();
    std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Compute ranking conflicts" << std::endl;
#endif
}

std::pair<std::pair<double, double>, std::pair<double, double>> Evaluator::get_confusion_matrix() {
    if (!confusion_matrix_computed_) compute_confusion_matrix();
    return confusion_matrix_;
}

double Evaluator::get_error_rate() {
    if (!confusion_matrix_computed_) compute_confusion_matrix();
    double const f = confusion_matrix_.first.second + confusion_matrix_.second.first;
    double const all = f +confusion_matrix_.first.first + confusion_matrix_.second.second;
    if (all == 0) return 0;
    return f / all;
}

double Evaluator::get_accuracy() {
    return 1 - get_error_rate();
}

double Evaluator::get_ranking_conflicts() {
    if (!ranking_conflicts_computed_) compute_ranking_conflicts();
    return ranking_conflicts_;
}

double Evaluator::get_roc_error() {
    double n=0, p=0;
    std::tie(n,p) = pSample_->get_neg_pos_counts();
    const double possible_conflicts = n*p;
    if (possible_conflicts == 0) return 0;
    return get_ranking_conflicts()/possible_conflicts;
}

double Evaluator::get_roc() {
    return 1-get_roc_error();
}

std::ostream &Evaluator::dump_results(std::ostream & os) {
    const unsigned int size = pSample_->size();
    for ( unsigned int i = 0; i < size; i++ ) {
        const auto &data = (*pSample_)[i]->get_data();
        const auto confidence_interval = pBorderSystem_->confidence_interval(data);
        os << *(*pSample_)[i] << " (" << confidence_interval.first << ", ";
        os << confidence_interval.second << ")" << std::endl;
    }
        return os;
}

std::ostream &Evaluator::evaluate_data_file(std::ostream & os, const std::string & data_file,
                                            const std::shared_ptr<FeatureNames> & pFN) const {
    if (pBorderSystem_ == nullptr)
        throw std::runtime_error("Set border system first");
    std::ifstream fs_data(data_file);
    if (!fs_data.is_open())
        throw std::runtime_error("Cannot open file: " + data_file);

    std::string line;
    while (std::getline(fs_data, line)) {
        line = std::regex_replace (line,std::regex("\r$"),"");
        const std::shared_ptr<FeatureVector> pFV =
                std::make_shared<FeatureVector>(line,
                                                pFN->get_feature_indices(),
                                                pFN->get_target_feature_index(),
                                                pFN->get_negatives_label(),
                                                pFN->get_positives_label(),
                                                pFN->get_weight_index());
        const auto &data = pFV->get_data();
        double a=0, b=0;
        std::tie(a,b) = pBorderSystem_->confidence_interval(data);
        std::string result_label;
        //a = b = (a+b)/2;
        if (a>0.5) {
            result_label = pFN->get_positives_label();
        } else if (b<0.5) {
            result_label = pFN->get_negatives_label();
        } else {
            result_label = "CLASS_UNDEFINED";
        }
        os << line << "," << result_label << ",";
        os << "(" << a << "," << b << ")";
        os << std::endl;
    }



    fs_data.close();


    return os;
}


#endif
