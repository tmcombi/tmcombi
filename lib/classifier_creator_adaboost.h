#ifndef LIB_CLASSIFIER_CREATOR_ADABOOST_H_
#define LIB_CLASSIFIER_CREATOR_ADABOOST_H_

///  AdaBoost

#include "classifier_creator_train.h"
#include "classifier_transformed_features.h"
#include "classifier_weighted_sum.h"
#include "evaluator.h"
#include "feature_transform_superposition.h"

class ClassifierDiscretizer : public Classifier {
public:
    explicit ClassifierDiscretizer( std::shared_ptr<const Classifier> pClassifier) : pClassifier_(std::move(pClassifier)) {};
    size_t dim() const override {return pClassifier_->dim();};
    double confidence(const std::vector<double> & v) const override {
        const double conf = pClassifier_->confidence(v);
        if (conf > 0.5) {
            return 1;
        } else if (conf < 0.5){
            return 0;
        }
        return conf;
    };
    void dump_to_ptree(boost::property_tree::ptree & pt) const override {
        using boost::property_tree::ptree;
        pt.put("type", "ClassifierDiscretizer");
        ptree cl;
        pClassifier_->dump_to_ptree(cl);
        pt.add_child("classifier", cl);
    };
private:
    std::shared_ptr<const Classifier> pClassifier_;
};

class ClassifierCreatorAdaboost : public ClassifierCreatorTrain {
public:
    ClassifierCreatorAdaboost();

    ClassifierCreatorAdaboost & init(const std::shared_ptr<Sample> &) override;
    ClassifierCreatorAdaboost & set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> &);

    ClassifierCreatorAdaboost & set_trials(size_t);
    ClassifierCreatorAdaboost & set_eval_sample(const std::shared_ptr<Sample> &);

    ClassifierCreatorAdaboost & train() override;

    std::shared_ptr<Classifier> get_classifier() const override;
private:
    std::shared_ptr<ClassifierCreatorTrain> pCCT_;
    std::shared_ptr<Sample> pSample_;
    std::vector<std::shared_ptr<Classifier>> v_pC_;
    std::vector<double> weights_;
    double bias_;
    std::shared_ptr<Classifier> pC_;
    size_t trials_;
    std::shared_ptr<Sample> pSampleEval_;
    bool trained_;

    std::shared_ptr<Sample> create_reweighted_sample_based_on_current_state();
    double compute_weight_of_the_last_classifier(const std::shared_ptr<Classifier>&);
};

ClassifierCreatorAdaboost::ClassifierCreatorAdaboost() : pCCT_(nullptr), pSample_(nullptr), v_pC_(0), weights_(0),
bias_(0), pC_(nullptr), trials_(4), pSampleEval_(nullptr), trained_(false) {
}

ClassifierCreatorAdaboost &ClassifierCreatorAdaboost::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    v_pC_.resize(0);
    weights_.resize(0);
    bias_ = 0;
    pC_ = nullptr;
    trained_ = false;
    return *this;
}

ClassifierCreatorAdaboost &ClassifierCreatorAdaboost::
set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> & pCCT) {
    if (pCCT_ != pCCT) {
        pCCT_ = pCCT;
        v_pC_.resize(0);
        weights_.resize(0);
        bias_ = 0;
        pC_ = nullptr;
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorAdaboost &ClassifierCreatorAdaboost::set_trials(const size_t trials) {
    if (trials_ != trials) {
        trials_ = trials;
        v_pC_.resize(0);
        weights_.resize(0);
        bias_ = 0;
        pC_ = nullptr;
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorAdaboost &ClassifierCreatorAdaboost::set_eval_sample(const std::shared_ptr<Sample> & pSampleEval) {
    pSampleEval_ = pSampleEval;
    return *this;
}

ClassifierCreatorAdaboost &ClassifierCreatorAdaboost::train() {
    if ( pCCT_ == nullptr ) throw std::runtime_error("run set_classifier_creator_train() prior to train");
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");
    if ( verbose() ) {
        double neg, pos;
        std::tie(neg,pos) = pSample->get_neg_pos_counts();
        std::cout << "Starting AdaBoost" << std::endl;
        std::cout << "Input Sample: size=" << pSample->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
        std::cout << "Training original classifier" << std::endl;
    }
    (*pCCT_).init(pSample).train();
    pC_ = std::make_shared<ClassifierDiscretizer>(pCCT_->get_classifier());
    v_pC_.push_back(pC_);
    weights_.push_back(1);
    auto pEvaluator = std::make_shared<Evaluator>();
    if ( verbose() ) {
        (*pEvaluator).set_conf_type(Evaluator::number);
        (*pEvaluator).set_classifier(pC_);
        (*pEvaluator).set_sample(pSample);
        (*pEvaluator).generate_report(std::cout,"trial 0, training sample");
        (*pEvaluator).set_sample(pSampleEval_);
        (*pEvaluator).generate_report(std::cout,"trial 0, evaluation sample");
    }
    for (size_t trial = 1; trial < trials_; trial++) {
        const auto pSampleReweighted = create_reweighted_sample_based_on_current_state();
        (*pCCT_).init(pSampleReweighted).train();
        std::shared_ptr<Classifier> pC = std::make_shared<ClassifierDiscretizer>(pCCT_->get_classifier());
        const double alpha = compute_weight_of_the_last_classifier(pC);
        v_pC_.push_back(pC);
        weights_.push_back(alpha);
        bias_ -= alpha/2;
        if (v_pC_.size() != weights_.size()) throw std::runtime_error("Unexpected error");
        const std::shared_ptr<FeatureTransform> pFT = std::make_shared<FeatureTransformSuperposition>(v_pC_);
        pC = std::make_shared<ClassifierWeightedSum>(weights_,bias_);
        pC_ = std::make_shared<ClassifierTransformedFeatures>(pC,pFT);
        if ( verbose() ) {
            std::cout << std::endl;
            (*pEvaluator).set_classifier(pC_);
            (*pEvaluator).set_sample(pSample);
            (*pEvaluator).generate_report(std::cout,"trial " + std::to_string(trial) + ", training sample");
            std::cout << "Weight of additionally trained weak classifier:" << alpha << std::endl;
            (*pEvaluator).set_sample(pSampleEval_);
            (*pEvaluator).generate_report(std::cout,"trial " + std::to_string(trial) + ", evaluation sample");
        }
    }
    trained_ = true;
    return *this;
}

std::shared_ptr<Classifier> ClassifierCreatorAdaboost::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return pC_;
}

std::shared_ptr<Sample> ClassifierCreatorAdaboost::create_reweighted_sample_based_on_current_state() {
    const auto pSample = get_sample();
    const size_t size = pSample->size();
    auto pSampleReweighted = std::make_shared<Sample>(pSample->dim());
    for (size_t i = 0; i < size; i++) {
        const auto & FV = *(*pSample)[i];
        auto pFV = std::make_shared<FeatureVector>(FV.get_data());
        const auto T = pC_->confidence(FV.get_data());
        const auto C = 2*T-1;
        const auto pos_factor = exp(-C);
        const auto neg_factor = exp(C);
        pFV->inc_weight_negatives(pos_factor*FV.get_weight_negatives());
        pFV->inc_weight_positives(neg_factor*FV.get_weight_positives());
        pSampleReweighted->push(pFV);
    }
    return pSampleReweighted;
}

double ClassifierCreatorAdaboost::compute_weight_of_the_last_classifier(const std::shared_ptr<Classifier>& pC) {
    const auto pSample = get_sample();
    const size_t size = pSample->size();
    double enumerator = 0, denominator = 0;
    for (size_t i = 0; i < size; i++) {
        const auto & FV = *(*pSample)[i];
        auto pFV = std::make_shared<FeatureVector>(FV.get_data());
        const auto T = pC_->confidence(FV.get_data());
        const auto C = 2*T-1;
        const auto pos_factor = exp(-C);
        const auto neg_factor = exp(C);
        const auto newT = pC->confidence(FV.get_data());
        if (newT > 0.5) {
            enumerator += pos_factor*FV.get_weight_positives();
            denominator += neg_factor*FV.get_weight_negatives();
        } else if (newT < 0.5) {
            denominator += pos_factor*FV.get_weight_positives();
            enumerator += neg_factor*FV.get_weight_negatives();
        } else {
            enumerator += pos_factor*FV.get_weight_positives()/2;
            denominator += neg_factor*FV.get_weight_negatives()/2;
            denominator += pos_factor*FV.get_weight_positives()/2;
            enumerator += neg_factor*FV.get_weight_negatives()/2;
        }
    }
    if (denominator == 0)
        throw std::runtime_error("AdaBoost denominator = 0 is not yet supported");
    return log(enumerator/denominator)/2;
}

#endif