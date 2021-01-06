#ifndef LIB_CLASSIFIER_CREATOR_FEATURE_SELECTION_H_
#define LIB_CLASSIFIER_CREATOR_FEATURE_SELECTION_H_

#include <random>
#include "classifier_creator_train.h"
#include "classifier_transformed_features.h"
#include "feature_transform_subset.h"
#include "evaluator.h"
#include "sample_creator.h"

class ClassifierCreatorFeatureSelection : public ClassifierCreatorTrain {
public:
    enum KPIType {roc_err, class_err};

    ClassifierCreatorFeatureSelection();

    ClassifierCreatorFeatureSelection & init(const std::shared_ptr<Sample> &) override;
    ClassifierCreatorFeatureSelection & set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> &);
    ClassifierCreatorFeatureSelection & set_n_folds(size_t);
    ClassifierCreatorFeatureSelection & set_seed(unsigned long);
    ClassifierCreatorFeatureSelection & set_kpi_type(KPIType);

    ClassifierCreatorFeatureSelection & train() override;

    std::shared_ptr<Classifier> get_classifier() const override;
private:
    std::shared_ptr<ClassifierCreatorTrain> pCCT_;
    std::vector<std::shared_ptr<Sample>> v_pSampleTrain_;
    std::vector<std::shared_ptr<Sample>> v_pSampleValidate_;
    std::shared_ptr<FeatureTransformSubset> pFT_;
    std::shared_ptr<Classifier> pC_;
    size_t n_folds_;
    unsigned long seed_;
    KPIType KPIType_;
    double best_target_kpi_;
    bool trained_;

    void create_n_samples_split  ();

    /// returns true if found a feature improving the performance
    bool check4additional_feature(boost::dynamic_bitset<> &, boost::dynamic_bitset<> &);

    /// returns { {roc_train_err, roc_eval_err}, {classification_train_err, classification_eval_err} }
    std::tuple<double, double, double, double> compute_kpi(const std::shared_ptr<FeatureTransform> &) const;
};

ClassifierCreatorFeatureSelection::ClassifierCreatorFeatureSelection() :
pCCT_(nullptr), v_pSampleTrain_(0), v_pSampleValidate_(0), pFT_(nullptr), pC_(nullptr), n_folds_(2), seed_(0),
KPIType_(roc_err), best_target_kpi_(std::numeric_limits<double>::max()), trained_(false) {
}

ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    v_pSampleTrain_.resize(0);
    v_pSampleValidate_.resize(0);
    pFT_ = nullptr;
    pC_ = nullptr;
    best_target_kpi_ = std::numeric_limits<double>::max();
    trained_ = false;
    return *this;
}

ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::
set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> & pCCT) {
    if (pCCT_ != pCCT) {
        pCCT_ = pCCT;
        v_pSampleTrain_.resize(0);
        v_pSampleValidate_.resize(0);
        pFT_ = nullptr;
        pC_ = nullptr;
        best_target_kpi_ = std::numeric_limits<double>::max();
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::set_n_folds(const size_t n_folds) {
    if (n_folds_ != n_folds) {
        if (n_folds < 2) throw std::runtime_error("number of folds must be at least 2");
        n_folds_ = n_folds;
        v_pSampleTrain_.resize(0);
        v_pSampleValidate_.resize(0);
        pFT_ = nullptr;
        pC_ = nullptr;
        best_target_kpi_ = std::numeric_limits<double>::max();
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::set_seed(const unsigned long seed) {
    if (seed_ != seed) {
        seed_ = seed;
        v_pSampleTrain_.resize(0);
        v_pSampleValidate_.resize(0);
        pFT_ = nullptr;
        pC_ = nullptr;
        best_target_kpi_ = std::numeric_limits<double>::max();
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::
set_kpi_type(ClassifierCreatorFeatureSelection::KPIType type) {
    if (type != KPIType_) {
        KPIType_ = type;
        v_pSampleTrain_.resize(0);
        v_pSampleValidate_.resize(0);
        pFT_ = nullptr;
        pC_ = nullptr;
        best_target_kpi_ = std::numeric_limits<double>::max();
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::train() {
    best_target_kpi_ = std::numeric_limits<double>::max();
    if ( pCCT_ == nullptr ) throw std::runtime_error("run set_classifier_creator_train() prior to train");
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");
    if ( n_folds_ > pSample->size() ) throw std::runtime_error("number of folds cannot be larger than the size of the sample");
    if ( verbose() ) {
        std::cout << "Starting feature selection" << std::endl;
        std::cout << "Creating " << n_folds_ << " sub-samples for cross validation" << std::endl;
        double neg, pos;
        std::tie(neg,pos) = pSample->get_neg_pos_counts();
        std::cout << "Input Sample: size=" << pSample->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
    }

    create_n_samples_split();

    if ( verbose() ) {
        double neg, pos;
        for (size_t i = 0; i < n_folds_; i++) {
            std::tie(neg,pos) = v_pSampleValidate_[i]->get_neg_pos_counts();
            std::cout << "Sample Validate[" << i << "]: size=" << v_pSampleValidate_[i]->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
            std::tie(neg,pos) = v_pSampleTrain_[i]->get_neg_pos_counts();
            std::cout << "Input Train[" << i << "]: size=" << v_pSampleTrain_[i]->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
        }
    }

    boost::dynamic_bitset<> feature_mask(pSample->dim());
    boost::dynamic_bitset<> sign_mask(pSample->dim()); /// 0 for "+" and 1 for "-"
    while (check4additional_feature(feature_mask, sign_mask));

    if (verbose()) {
        std::cout << std::endl;
        std::cout << "###################################################################################" << std::endl;
        std::cout << "Feature selection finished: ";
        std::cout << "feature_mask = \"" << boost::to_string(feature_mask) << "\", ";
        std::cout << "sign_mask = \"" << boost::to_string(sign_mask) << "\"" << std::endl;
        std::cout << "Training now the classifier with the selected features on the whole sample without folding";
        std::cout << std::endl;
    }

    pFT_ = std::make_shared<FeatureTransformSubset>(feature_mask,sign_mask);
    const auto pSampleTransformed = SampleCreator::transform_features(pSample, pFT_);
    (*pCCT_).init(pSampleTransformed).train();
    pC_ = pCCT_->get_classifier();

    trained_ = true;
    return *this;
}

std::shared_ptr<Classifier> ClassifierCreatorFeatureSelection::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTransformedFeatures>(pC_,pFT_);
}

void ClassifierCreatorFeatureSelection::create_n_samples_split() {
    std::default_random_engine generator(seed_);
    const auto pSample = get_sample();
    v_pSampleTrain_.resize(n_folds_);
    v_pSampleValidate_.resize(n_folds_);
    for (size_t i = 0; i < n_folds_; i++) {
        v_pSampleTrain_[i] = std::make_shared<Sample>(pSample->dim());
        v_pSampleValidate_[i] = std::make_shared<Sample>(pSample->dim());
    }
    std::vector<size_t> permutation(pSample->size());
    std::iota (std::begin(permutation), std::end(permutation), 0);
    std::shuffle (permutation.begin(), permutation.end(), generator);
    for (size_t j = 0; j < pSample->size(); j++)
        for (size_t k = 0; k < n_folds_; k++)
            if (k == j % n_folds_)
                v_pSampleValidate_[k]->push((*pSample)[permutation[j]]);
            else
                v_pSampleTrain_[k]->push((*pSample)[permutation[j]]);
}

bool ClassifierCreatorFeatureSelection::check4additional_feature(boost::dynamic_bitset<> & feature_mask,
                                                                 boost::dynamic_bitset<> & sign_mask) {
    double roc_train_err, roc_eval_err, classification_train_err, classification_eval_err;
    double target_kpi;
    size_t best_feature_index = feature_mask.size();
    bool best_sign;
    for(size_t i=0; i < feature_mask.size(); i++) {
        if (feature_mask[i]) continue;
        const bool signs[] = {false, true};
        for (bool sign: signs) {
            feature_mask[i] = true;
            sign_mask[i] = sign;
            if (verbose()) {
                std::cout << std::endl;
                std::cout << "checking feature " << i << " with sign " << (sign ? "\"-\"" : "\"+\"") << ":";
                std::cout << " feature_mask = \"" << boost::to_string(feature_mask) << "\", ";
                std::cout << "sign_mask = \"" << boost::to_string(sign_mask) << "\"" << std::endl;
            }
            auto pFT = std::make_shared<FeatureTransformSubset>(feature_mask,sign_mask);
            std::tie(roc_train_err, roc_eval_err, classification_train_err, classification_eval_err) =
                    compute_kpi(pFT);
            if (verbose()) {
                std::cout << "roc_train_err = " << roc_train_err;
                std::cout << ", roc_eval_err = " << roc_eval_err;
                std::cout << ", classification_train_err = " << classification_train_err;
                std::cout << ", classification_eval_err = " << classification_eval_err;
                std::cout << std::endl;
            }
            if (KPIType_ == roc_err)
                target_kpi = roc_eval_err;
            else if (KPIType_ == class_err)
                target_kpi = classification_eval_err;
            else throw std::runtime_error("KPI type is not supported yet");
            if (target_kpi < best_target_kpi_) {
                best_target_kpi_ = target_kpi;
                best_feature_index = i;
                best_sign = sign;
            }
            feature_mask[i] = false;
            sign_mask[i] = false;
        }
    }
    if ( best_feature_index < feature_mask.size() ) {
        feature_mask[best_feature_index] = true;
        sign_mask[best_feature_index] = best_sign;
        if (verbose()) {
            std::cout << std::endl;
            std::cout << "###################################################################################" << std::endl;
            std::cout << "Successfully taken feature " << best_feature_index;
            std::cout << " with sign " << (best_sign ? "\"-\"" : "\"+\"") << ": ";
            std::cout << "feature_mask = \"" << boost::to_string(feature_mask) << "\", ";
            std::cout << "sign_mask = \"" << boost::to_string(sign_mask) << "\"" << std::endl;
            std::cout << std::endl;
        }
        return true;
    } else {
        if (verbose()) {
            std::cout << std::endl;
            std::cout << "###################################################################################" << std::endl;
            std::cout << "Additional feature could not be found";
            std::cout << std::endl;
        }
    }
    return false;
}

/// returns { roc_train_err, roc_eval_err, classification_train_err, classification_eval_err }
std::tuple<double, double, double, double> ClassifierCreatorFeatureSelection::
compute_kpi(const std::shared_ptr<FeatureTransform> & pFT) const {
    double roc_train_err = 0, roc_eval_err = 0, classification_train_err = 0, classification_eval_err = 0;
    for (size_t i = 0; i < n_folds_; i++) {
        const auto pSampleTrain = SampleCreator::transform_features(v_pSampleTrain_[i],pFT);
        const auto pSampleEval = SampleCreator::transform_features(v_pSampleValidate_[i],pFT);
        const auto pC = (*pCCT_).init(pSampleTrain).train().get_classifier();
        auto pEvaluator = std::make_shared<Evaluator>();
        (*pEvaluator).set_classifier(pC);
        (*pEvaluator).set_sample(pSampleTrain);
        const double roc_train_err_fold = pEvaluator->get_roc_error();
        const double classification_train_err_fold =pEvaluator->get_error_rate();
        roc_train_err += roc_train_err_fold;
        classification_train_err += classification_train_err_fold;
        (*pEvaluator).set_sample(pSampleEval);
        const double roc_eval_err_fold = pEvaluator->get_roc_error();
        const double classification_eval_err_fold =pEvaluator->get_error_rate();
        roc_eval_err += roc_eval_err_fold;
        classification_eval_err += classification_eval_err_fold;
        if(verbose()) {
            std::cout << "Fold[" << i << "]: ";
            std::cout << "roc_train_err = " << roc_train_err_fold;
            std::cout << ", roc_eval_err = " << roc_eval_err_fold;
            std::cout << ", classification_train_err = " << classification_train_err_fold;
            std::cout << ", classification_eval_err = " << classification_eval_err_fold;
            std::cout << std::endl;
        }
    }
    return {roc_train_err/n_folds_, roc_eval_err/n_folds_,
            classification_train_err/n_folds_, classification_eval_err/n_folds_};
}

#endif