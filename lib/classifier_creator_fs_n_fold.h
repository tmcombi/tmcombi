#ifndef LIB_CLASSIFIER_CREATOR_FS_N_FOLD_H_
#define LIB_CLASSIFIER_CREATOR_FS_N_FOLD_H_

///  feature forward selection using n-fold cross validation

#include "classifier_creator_fs.h"
#include "evaluator.h"
#include "sample_creator.h"

class ClassifierCreatorFsNfold : public ClassifierCreatorFs {
public:
    enum KPIType {roc_err, class_err};

    ClassifierCreatorFsNfold();

    ClassifierCreatorFsNfold & init(const std::shared_ptr<Sample> &) override;

    ClassifierCreatorFsNfold & set_n_folds(size_t);
    ClassifierCreatorFsNfold & set_seed(unsigned long);
    ClassifierCreatorFsNfold & set_kpi_type(KPIType);

private:
    std::vector<std::shared_ptr<Sample>> v_pSampleTrain_;
    std::vector<std::shared_ptr<Sample>> v_pSampleValidate_;
    size_t n_folds_;
    unsigned long seed_;
    KPIType KPIType_;
    double best_target_kpi_;

    /// returns true if found a feature improving the performance
    bool check4additional_feature(const std::shared_ptr<FeatureMask> &);

    /// returns { {roc_train_err, roc_eval_err}, {classification_train_err, classification_eval_err} }
    std::tuple<double, double, double, double> compute_kpi(const std::shared_ptr<const FeatureTransform> &) const;

    void select(const std::shared_ptr<FeatureMask> &) override;

    void reset() override;
};

ClassifierCreatorFsNfold::ClassifierCreatorFsNfold() : v_pSampleTrain_(0), v_pSampleValidate_(0),
n_folds_(2), seed_(0), KPIType_(roc_err), best_target_kpi_(std::numeric_limits<double>::max()) {
}

ClassifierCreatorFsNfold &ClassifierCreatorFsNfold::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorFs::init(pSample);
    reset();
    return *this;
}

ClassifierCreatorFsNfold &ClassifierCreatorFsNfold::set_n_folds(const size_t n_folds) {
    if (n_folds_ != n_folds) {
        if (n_folds < 2) throw std::runtime_error("number of folds must be at least 2");
        n_folds_ = n_folds;
        reset();
    }
    return *this;
}

ClassifierCreatorFsNfold &ClassifierCreatorFsNfold::set_seed(const unsigned long seed) {
    if (seed_ != seed) {
        seed_ = seed;
        reset();
    }
    return *this;
}

ClassifierCreatorFsNfold &ClassifierCreatorFsNfold::
set_kpi_type(ClassifierCreatorFsNfold::KPIType type) {
    if (type != KPIType_) {
        KPIType_ = type;
        reset();
    }
    return *this;
}

void ClassifierCreatorFsNfold::select(const std::shared_ptr<FeatureMask> & pFM) {
    best_target_kpi_ = std::numeric_limits<double>::max();
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");
    if ( n_folds_ > pSample->size() ) throw std::runtime_error("number of folds cannot be larger than the size of the sample");
    if ( verbose() ) {
        std::cout << "Creating " << n_folds_ << " sub-samples for cross validation" << std::endl;
    }

    SampleCreator::create_n_samples_split(pSample,n_folds_,seed_,v_pSampleValidate_);
    v_pSampleTrain_.resize(n_folds_);
    for (size_t i = 0; i < n_folds_; i++) {
        v_pSampleTrain_[i] = std::make_shared<Sample>(pSample->dim());
        for (size_t j = 0; j < n_folds_; j++) {
            if (j != i) {
                v_pSampleTrain_[i]->push(v_pSampleValidate_[j]);
            }
        }
    }

    if ( verbose() ) {
        double neg, pos;
        for (size_t i = 0; i < n_folds_; i++) {
            std::tie(neg,pos) = v_pSampleValidate_[i]->get_neg_pos_counts();
            std::cout << "Sample Validate[" << i << "]: size=" << v_pSampleValidate_[i]->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
            std::tie(neg,pos) = v_pSampleTrain_[i]->get_neg_pos_counts();
            std::cout << "Input Train[" << i << "]: size=" << v_pSampleTrain_[i]->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
        }
    }

    if (pFM->count() > 0) {
        if (verbose()) {
            std::cout << "Started feature selection with non-empty feature choice: ";
            const auto fm_pair = pFM->to_strings();
            std::cout << " feature_mask = \"" << fm_pair.first << "\", ";
            std::cout << "sign_mask = \"" << fm_pair.second << "\"" << std::endl;
        }
        double roc_train_err, roc_eval_err, classification_train_err, classification_eval_err;
        auto pFT = std::make_shared<FeatureTransformSubset>(pFM);
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
            best_target_kpi_ = roc_eval_err;
        else if (KPIType_ == class_err)
            best_target_kpi_ = classification_eval_err;
        else throw std::runtime_error("KPI type is not supported yet");
    }

    while (check4additional_feature(pFM));
}

bool ClassifierCreatorFsNfold::check4additional_feature(const std::shared_ptr<FeatureMask> & pFM) {
    double roc_train_err, roc_eval_err, classification_train_err, classification_eval_err;
    double target_kpi;
    size_t best_feature_index = pFM->dim();
    bool best_sign = false;
#if 0
    std::tie(best_feature_index, best_sign) = analyze_current_mask(pFM);
#else
    for(size_t i=0; i < pFM->dim(); i++) {
        if ((*pFM)[i]) continue;
        const bool signs[] = {false, true};
        for (bool sign: signs) {
            (*pFM)[i] = true;
            pFM->sign(i) = sign;
            if (verbose()) {
                const auto fm_pair = pFM->to_strings();
                std::cout << std::endl;
                std::cout << "checking feature " << i << " with sign " << (sign ? "\"-\"" : "\"+\"") << ":";
                std::cout << " feature_mask = \"" << fm_pair.first << "\", ";
                std::cout << "sign_mask = \"" << fm_pair.second << "\"" << std::endl;
            }
            auto pFT = std::make_shared<FeatureTransformSubset>(pFM);
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
            (*pFM)[i] = false;
            pFM->sign(i) = false;
        }
    }
#endif
    if ( best_feature_index < pFM->dim() ) {
        (*pFM)[best_feature_index] = true;
        pFM->sign(best_feature_index) = best_sign;
        if (verbose()) {
            const auto fm_pair = pFM->to_strings();
            std::cout << std::endl;
            std::cout << "###################################################################################" << std::endl;
            std::cout << "Successfully taken feature " << best_feature_index;
            std::cout << " with sign " << (best_sign ? "\"-\"" : "\"+\"") << ": ";
            std::cout << "feature_mask = \"" << fm_pair.first << "\", ";
            std::cout << "sign_mask = \"" << fm_pair.second << "\"" << std::endl;
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
std::tuple<double, double, double, double> ClassifierCreatorFsNfold::
compute_kpi(const std::shared_ptr<const FeatureTransform> & pFT) const {
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

void ClassifierCreatorFsNfold::reset() {
    ClassifierCreatorFs::reset();
    v_pSampleTrain_.resize(0);
    v_pSampleValidate_.resize(0);
    best_target_kpi_ = std::numeric_limits<double>::max();
}

#endif