#ifndef LIB_EVALUATOR_H_
#define LIB_EVALUATOR_H_

#include "sample.h"
#include "border_system.h"


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

private:
    std::shared_ptr<Sample> pSample_;
    std::shared_ptr<BorderSystem> pBorderSystem_;
    bool confusion_matrix_computed_;
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_;
    bool ranking_conflicts_computed_;
    double ranking_conflicts_;

    void compute_confusion_matrix();
    void compute_ranking_conflicts();
};

Evaluator::Evaluator() : pSample_(nullptr), pBorderSystem_(nullptr),
                         confusion_matrix_computed_(false), confusion_matrix_({{0,0},{0,0}}),
                         ranking_conflicts_computed_(false), ranking_conflicts_(0)
{}

Evaluator &Evaluator::set_sample(const std::shared_ptr<Sample> & pSample) {
    pSample_ = pSample;
    confusion_matrix_computed_ = false;
    ranking_conflicts_computed_ = false;
    confusion_matrix_ = std::pair<std::pair<double, double>, std::pair<double, double>> ({{0,0},{0,0}});
    ranking_conflicts_ = 0;
    return *this;
}

Evaluator &Evaluator::set_border_system(const std::shared_ptr<BorderSystem> & pBorderSystem) {
    pBorderSystem_ = pBorderSystem;
    confusion_matrix_computed_ = false;
    ranking_conflicts_computed_ = false;
    confusion_matrix_ = std::pair<std::pair<double, double>, std::pair<double, double>> ({{0,0},{0,0}});
    ranking_conflicts_ = 0;
    return *this;
}

void Evaluator::compute_confusion_matrix() {
    const unsigned int size = pSample_->size();
    confusion_matrix_.first.first = confusion_matrix_.first.second =
            confusion_matrix_.second.first = confusion_matrix_.second.second = 0;
    double c1=0, c2=0;
    for ( unsigned int i = 0; i < size; i++ ) {
        const auto & data = (*pSample_)[i]->get_data();
        std::tie(c1,c2) = pBorderSystem_->confidence_interval(data);
        if (c1 > 0.5) {
            confusion_matrix_.first.first += (*pSample_)[i]->get_weight_positives();
            confusion_matrix_.first.second += (*pSample_)[i]->get_weight_negatives();
        } else if (c2 < 0.5) {
            confusion_matrix_.second.first += (*pSample_)[i]->get_weight_positives();
            confusion_matrix_.second.second += (*pSample_)[i]->get_weight_negatives();
        } else {
            confusion_matrix_.first.first += (*pSample_)[i]->get_weight_positives()/2;
            confusion_matrix_.first.second += (*pSample_)[i]->get_weight_negatives()/2;
            confusion_matrix_.second.first += (*pSample_)[i]->get_weight_positives()/2;
            confusion_matrix_.second.second += (*pSample_)[i]->get_weight_negatives()/2;
        }
    }
    confusion_matrix_computed_ = true;
}

void Evaluator::compute_ranking_conflicts() {
    ranking_conflicts_ = 0;
    double c1=0, c2=0, d1=0, d2=0;
    const unsigned int size = pSample_->size();
    for ( unsigned int i = 0; i < size; i++ ) {
        std::tie(c1,c2) = pBorderSystem_->confidence_interval((*pSample_)[i]->get_data());
        for ( unsigned int j = 0; j < size; j++ ) {
            std::tie(d1,d2) = pBorderSystem_->confidence_interval((*pSample_)[j]->get_data());
            if (d1>c2) {
                ranking_conflicts_ += (*pSample_)[j]->get_weight_negatives() * (*pSample_)[i]->get_weight_positives();
            } else if (d2<c1) {
                ranking_conflicts_ += (*pSample_)[i]->get_weight_negatives() * (*pSample_)[j]->get_weight_positives();
            } else {
                ranking_conflicts_ += (
                        (*pSample_)[j]->get_weight_negatives() * (*pSample_)[i]->get_weight_positives()
                        + (*pSample_)[i]->get_weight_negatives() * (*pSample_)[j]->get_weight_positives()
                )/2;
            }
        }
    }
    ranking_conflicts_ /= 2;
    ranking_conflicts_computed_ = true;
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

#endif
