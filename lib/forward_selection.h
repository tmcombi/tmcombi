#ifndef LIB_FORWARD_SELECTION_H_
#define LIB_FORWARD_SELECTION_H_

#define DEFAULT_OBJECTIVE_KPI roc_err
//#define DEFAULT_OBJECTIVE_KPI err_rate

#define DEFAULT_EVAL_PROPORTION 0.33
#define DEFAULT_SEED 0

#include "sample_creator.h"
#include "layer_partitioning_creator.h"
#include "border_system_creator.h"
#include "evaluator.h"

class ForwardSelection {
public:

    enum KpiType {err_rate, roc_err};

    explicit ForwardSelection(std::ostream & os = std::cout);
    void set_sample(const std::shared_ptr<Sample> &);
    void set_objective_kpi_type(KpiType);
    void set_eval_proportion(double);
    void set_seed(unsigned long);

    void init();
    bool try_inactive_features();

    /// Input: index to check, sing flag false="+", true="-"
    /// Output: objective_kpi_value, corresponding BorderSystem
    std::pair<double, std::shared_ptr<BorderSystem> >  try_inactive_feature(unsigned int, bool);

    /// Output: feature mask, feature sign mask
    std::pair<boost::dynamic_bitset<>,boost::dynamic_bitset<> > optimize();
private:
    std::ostream & os_;
    std::shared_ptr<Sample> pSample_;
    std::shared_ptr<Sample> pSampleTrain_;
    std::shared_ptr<Sample> pSampleEval_;

    double eval_proportion_;
    unsigned long seed_;

    boost::dynamic_bitset<> selected_features_;
    boost::dynamic_bitset<> selected_features_sign_; /// 0="+", 1="-"

    std::multimap<double, unsigned int> confidence2index_map_;
    KpiType objective_kpi_type_;
    double objective_kpi_value_;

    bool setter_allowed_;
};

ForwardSelection::ForwardSelection(std::ostream & os) :
os_(os), pSample_(nullptr),
pSampleTrain_(nullptr), pSampleEval_(nullptr), eval_proportion_(DEFAULT_EVAL_PROPORTION), seed_(DEFAULT_SEED),
objective_kpi_type_(DEFAULT_OBJECTIVE_KPI), objective_kpi_value_(std::numeric_limits<double>::max()),
setter_allowed_(true)
{
    os_ << "FS: entering forward selection (FS) algorithm" << std::endl;
}

void ForwardSelection::set_sample(const std::shared_ptr<Sample> &pSample) {
    if (!setter_allowed_)
        throw std::runtime_error("set-functions are not allowed any more");
    pSample_ = pSample;
    selected_features_.resize(pSample->dim(), false);
    selected_features_sign_.resize(pSample->dim(), false);
    os_ << "FS: setting the sample of size " << pSample->size() << std::endl;
}

void ForwardSelection::set_objective_kpi_type(KpiType objective_kpi_type) {
    if (!setter_allowed_)
        throw std::runtime_error("set-functions are not allowed any more");
    objective_kpi_type_ = objective_kpi_type;
}

void ForwardSelection::set_eval_proportion(double eval_proportion) {
    if (!setter_allowed_)
        throw std::runtime_error("set-functions are not allowed any more");
    eval_proportion_ = eval_proportion;
}

void ForwardSelection::set_seed(unsigned long seed) {
    if (!setter_allowed_)
        throw std::runtime_error("set-functions are not allowed any more");
    seed_ = seed;
}

void ForwardSelection::init() {
    if (!pSample_)
        throw std::runtime_error("Use set_sample to set an input sample prior init()");
    setter_allowed_ = false;
    os_ << "FS: splitting the sample into train and evaluation parts" << std::endl;
    std::tie(pSampleTrain_,pSampleEval_) = SampleCreator::split2train_eval(pSample_,eval_proportion_,seed_);
    os_ << "FS: train sample size = " << pSampleTrain_->size() << std::endl;
    os_ << "FS: eval  sample size = " << pSampleEval_->size() << std::endl;
    os_ << "FS: set the starting confidence map (no active features -> all confidences = 0.5)" << std::endl;
    confidence2index_map_.clear();
    for (unsigned int i = 0; i < pSampleTrain_->size(); i++) {
        confidence2index_map_.insert(std::make_pair(0.5, i));
    }
}

bool ForwardSelection::try_inactive_features() {
    const unsigned int dim = pSample_->dim();
    if (selected_features_.count() == dim) return false;
    double objective_kpi = std::numeric_limits<double>::max();
    double best_found_objective_kpi = std::numeric_limits<double>::max();
    unsigned int best_found_index = 0;
    bool best_found_sign = false;
    std::shared_ptr<BorderSystem> pBS, pBestBS;
    for (unsigned int i=0; i<dim; i++) {
        if (selected_features_[i]) continue;
        const bool signs[] = {false, true};
        for (bool sign: signs) {
            std::tie(objective_kpi, pBS) = try_inactive_feature(i, sign);
            if (objective_kpi < best_found_objective_kpi) {
                best_found_objective_kpi = objective_kpi;
                best_found_index = i;
                best_found_sign = sign;
                pBestBS = pBS;
            }
        }
    }
    if (best_found_objective_kpi >= objective_kpi_value_) {
        os_ << std::endl << "FS: accept neither of still available features" << std::endl;
        return false;
    }
    objective_kpi_value_ = best_found_objective_kpi;
    selected_features_[best_found_index] = true;
    selected_features_sign_[best_found_index] = best_found_sign;

    os_ << "FS: next feature accepted with index " << best_found_index;
    os_ << " and sign " << "\"" << (best_found_sign ? "-" : "+") << "\"";
    os_ << ", feature mask = " << boost::to_string(selected_features_);
    os_ << ", sign mask = " << boost::to_string(selected_features_sign_) << std::endl;

    if (selected_features_.count() < dim) {
        os_ << "FS: Computing the confidence map for next iterations ";
        std::clock_t time1 = std::clock();

        std::vector<unsigned int> feature_indices;
        std::vector<bool> signs;
        for (unsigned int i = 0; i < selected_features_.size(); i++) {
            if (selected_features_[i]) {
                feature_indices.push_back(i);
                signs.push_back(selected_features_sign_[i]);
            }
        }

        confidence2index_map_.clear();
        double a = 0, b = 1;
        for (unsigned int i = 0; i < pSampleTrain_->size(); i++) {
            std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                    (*(*pSampleTrain_)[i], feature_indices, signs);
            std::tie(a, b) = pBestBS->confidence_interval(pFV->get_data());
            if (a != b)
                throw std::runtime_error(
                        "for a train sample every confidence interval is expected to be of zero length");
            confidence2index_map_.insert(std::make_pair(a, i));
        }

        std::clock_t time2 = std::clock();
        os_ << "(" << (time2 - time1) / (CLOCKS_PER_SEC / 1000) << "ms)" << std::endl;
    }

    return true;
}

std::pair<double, std::shared_ptr<BorderSystem> > ForwardSelection::
try_inactive_feature(const unsigned int index, const bool sign) {
    boost::dynamic_bitset<> active_features(selected_features_);
    boost::dynamic_bitset<> active_features_sign(selected_features_sign_);
    active_features[index] = true;
    active_features_sign[index] = sign;

    os_ << std::endl << "FS: start trying feature " << index;
    os_ << " with sign " << "\"" << (sign ? "-" : "+") << "\"";
    os_ << ", feature mask = " << boost::to_string(active_features);
    os_ << ", sign mask = " << boost::to_string(active_features_sign);
    std::vector<unsigned int> feature_indices;
    std::vector<bool> signs;

    for (unsigned int i=0; i < active_features.size(); i++) {
        if (active_features[i]) {
            feature_indices.push_back(i);
            signs.push_back(active_features_sign[i]);
        }
    }
    const unsigned int dim = feature_indices.size();
    std::shared_ptr<Sample> pSampleTrain = std::make_shared<Sample>(dim);
    std::shared_ptr<Sample> pSampleEval = std::make_shared<Sample>(dim);

    for (unsigned int i=0; i < pSampleTrain_->size(); i++) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                (*(*pSampleTrain_)[i], feature_indices, signs);
        pSampleTrain->push(pFV);
    }
    os_ << ", train size = " << pSampleTrain->size();

    for (unsigned int i=0; i < pSampleEval_->size(); i++) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                (*(*pSampleEval_)[i], feature_indices, signs);
        pSampleEval->push(pFV);
    }
    os_ << ", eval size = " << pSampleEval->size() << std::endl;

    os_ << "FS: use available border system to compute initial splitting of the sample: " << pSampleTrain->size() << " = ";

    std::shared_ptr<LayerPartitioningCreator> pLPC = std::make_shared<LayerPartitioningCreator>();
    std::shared_ptr<Sample> pCurrentSample = std::make_shared<Sample>(dim);
    unsigned int total_pushed_size = 0;
    double feature_value = confidence2index_map_.begin()->first;
    for (auto it = confidence2index_map_.begin(); it!=confidence2index_map_.end(); ++it) {
        if (it->first == feature_value) {
            const unsigned int i = it->second;
            std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                    (*(*pSampleTrain_)[i], feature_indices, signs);
            pCurrentSample->push(pFV);
        } else {
            total_pushed_size += pCurrentSample->size();
            pLPC->push_back(pCurrentSample);
            os_  << pCurrentSample->size() << " + ";
            pCurrentSample = std::make_shared<Sample>(dim);
            feature_value = it->first;
            const unsigned int i = it->second;
            std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                    (*(*pSampleTrain_)[i], feature_indices, signs);
            pCurrentSample->push(pFV);
        }
    }
    total_pushed_size += pCurrentSample->size();
    pLPC->push_back(pCurrentSample);
    os_ << pCurrentSample->size() << std::endl;
    if (total_pushed_size != pSampleTrain->size())
        throw std::runtime_error("something went wrong: counting feature vector does not coincide with the sample size");

    os_ << "FS: Start tmc optimization" << std::endl;
    std::clock_t time1 = std::clock();
    const unsigned int num_iterations = pLPC->optimize();
    const std::shared_ptr<LayerPartitioning> pLP = pLPC->get_layer_partitioning();
    std::clock_t time2 = std::clock();
    os_ << "FS: Finish tmc optimization in " << num_iterations << " iterations (" << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms)";
    os_ << ", size = " << pLP->size() << " ( ";
    unsigned int neg=0, pos=0;
    for (auto it=pLP->begin(); it!=pLP->end(); ++it) {
        std::tie(neg,pos) = (*it)->get_neg_pos_counts();
        os_ << neg << "|" << pos << " ";
    }
    os_ << ")" << std::endl;

    os_ << "FS: Start border system creation" << std::endl;
    time1 = std::clock();
    const auto pBSC = std::make_shared<BorderSystemCreator>();
    const auto pBS = pBSC->from_layer_partitioning(pLP);
    time2 = std::clock();
    os_ << "FS: Finish border system creation (" << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms)" << std::endl;

    std::shared_ptr<Evaluator> pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_border_system(pBS);
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;

    (*pEvaluator).set_sample(pSampleTrain);
    roc_err_train = pEvaluator->get_roc_error();
    err_rate_train = pEvaluator->get_error_rate();

    (*pEvaluator).set_sample(pSampleEval);
    roc_err_eval = pEvaluator->get_roc_error();
    err_rate_eval = pEvaluator->get_error_rate();

    os_ << "FS: roc_err_train = " << roc_err_train;
    os_ << ", roc_err_eval = " << roc_err_eval;
    os_ << ", err_rate_train = " << err_rate_train;
    os_ << ", err_rate_eval = " << err_rate_eval << std::endl;

    if (objective_kpi_type_ == roc_err) {
        return {roc_err_eval, pBS};
    } else if (objective_kpi_type_ == err_rate) {
        return {err_rate_eval, pBS};
    } else {
        throw std::domain_error("Not supported kpi");
    }
}

std::pair<boost::dynamic_bitset<>,boost::dynamic_bitset<> > ForwardSelection::optimize() {
    if (!pSample_)
        throw std::runtime_error("Use set_sample to set an input sample prior init()");
    if (!pSampleTrain_ || !pSampleEval_) init();
    while ( try_inactive_features() )
        os_ << std::endl << "##########################################################################" << std::endl;
    os_ << std::endl << "FS: Feature Selection finished, feature mask = " << boost::to_string(selected_features_);
    os_ << ", sign mask = " << boost::to_string(selected_features_sign_) << std::endl;
    return {selected_features_,selected_features_sign_};
}


#endif
