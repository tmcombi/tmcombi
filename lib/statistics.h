#ifndef LIB_STATISTICS_H_
#define LIB_STATISTICS_H_


#include "sample.h"

class Statistics {
public:
    Statistics();
    Statistics & set_sample(const std::shared_ptr<Sample> &);
    std::vector<double> get_roc_err_feature_wise();

private:
    std::shared_ptr<Sample> pSample_;

    bool roc_err_feature_wise_computed_;
    std::vector<double> roc_err_feature_wise_;
    std::vector<double> ranking_conflicts_;
    void compute_roc_err_feature_wise();

};

Statistics::Statistics() : pSample_(nullptr), roc_err_feature_wise_computed_(false) {
}

Statistics &Statistics::set_sample(const std::shared_ptr<Sample> & pSample) {
    pSample_ = pSample;
    roc_err_feature_wise_computed_ = false;
    roc_err_feature_wise_.clear();
    ranking_conflicts_.clear();
    return *this;
}

std::vector<double> Statistics::get_roc_err_feature_wise() {
    if (!roc_err_feature_wise_computed_)
        compute_roc_err_feature_wise();
    return roc_err_feature_wise_;
}

void Statistics::compute_roc_err_feature_wise() {
    const size_t dim = pSample_->dim();
    roc_err_feature_wise_.assign(dim, 0);
    ranking_conflicts_.assign(dim, 0);
    std::vector<std::map<double, std::pair<double, double> > > feature_confidence2negpos_map(dim);

    const size_t size = pSample_->size();
    for (size_t i=0; i<size; i++) {
        const auto & v = *(*pSample_)[i];
        for (size_t j=0; j<dim; j++) {
            auto & map_j = feature_confidence2negpos_map[j];
            auto it = map_j.find(v[j]);
            if (it == map_j.end()) {
                map_j[v[j]] = {v.get_weight_negatives(), v.get_weight_positives()};
            } else {
                map_j[v[j]].first += v.get_weight_negatives();
                map_j[v[j]].second += v.get_weight_positives();
            }
        }
    }

    double total_neg=0, total_pos=0;
    std::tie(total_neg,total_pos) = pSample_->get_neg_pos_counts();
    const double possible_conflicts = total_neg*total_pos;

    for (size_t j=0; j<dim; j++) {
        double current_neg_count=0, current_pos_count=0;
        for ( auto it = feature_confidence2negpos_map[j].begin(); it != feature_confidence2negpos_map[j].end(); ++it) {
            const double n = it->second.first; const double p = it->second.second;
            ranking_conflicts_[j] += n * p / 2;
            ranking_conflicts_[j] += n * current_pos_count;
            current_neg_count += n;
            current_pos_count += p;
        }
        if (possible_conflicts != 0)
            roc_err_feature_wise_[j] = ranking_conflicts_[j]/possible_conflicts;
    }
    roc_err_feature_wise_computed_ = true;
}


#endif
