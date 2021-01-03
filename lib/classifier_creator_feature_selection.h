#ifndef LIB_CLASSIFIER_CREATOR_FEATURE_SELECTION_H_
#define LIB_CLASSIFIER_CREATOR_FEATURE_SELECTION_H_

#include "classifier_creator_train.h"
#include "classifier_transformed_features.h"
#include "feature_transform_subset.h"

class ClassifierCreatorFeatureSelection : public ClassifierCreatorTrain {
public:
    enum FoldingType {split, weights};

    ClassifierCreatorFeatureSelection();

    ClassifierCreatorFeatureSelection & init(const std::shared_ptr<Sample> &) override;
    ClassifierCreatorFeatureSelection & set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> &);
    ClassifierCreatorFeatureSelection & set_n_folds(size_t);
    ClassifierCreatorFeatureSelection & set_folding_type(FoldingType);


    ClassifierCreatorFeatureSelection & train() override;

    std::shared_ptr<Classifier> get_classifier() const override;
private:
    std::shared_ptr<ClassifierCreatorTrain> pCCT_;
    std::vector<std::shared_ptr<Sample>> v_pSampleTrain_;
    std::vector<std::shared_ptr<Sample>> v_pSampleValidate_;
    std::shared_ptr<FeatureTransformSubset> pFT_;
    std::shared_ptr<Classifier> pC_;
    size_t n_folds_;
    FoldingType FoldingType_;
    bool trained_;

    void create_n_samples_weights();
    void create_n_samples_split  ();

    void generate_weights(std::vector<double> &,  std::default_random_engine &) const;
};

ClassifierCreatorFeatureSelection::ClassifierCreatorFeatureSelection() :
pCCT_(nullptr), v_pSampleTrain_(0), v_pSampleValidate_(0), pFT_(nullptr), pC_(nullptr),
n_folds_(2), FoldingType_(split), trained_(false) {
}

ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    v_pSampleTrain_.resize(0);
    v_pSampleValidate_.resize(0);
    pFT_ = nullptr;
    pC_ = nullptr;
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
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::
set_folding_type(ClassifierCreatorFeatureSelection::FoldingType type) {
    if (type != FoldingType_) {
        FoldingType_ = type;
        v_pSampleTrain_.resize(0);
        v_pSampleValidate_.resize(0);
        pFT_ = nullptr;
        pC_ = nullptr;
        trained_ = false;
    }
    return *this;
}


ClassifierCreatorFeatureSelection &ClassifierCreatorFeatureSelection::train() {
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
    if (FoldingType_ == split) {
        create_n_samples_split();
    } else if (FoldingType_ == weights) {
        create_n_samples_weights();
    } else {
        throw std::runtime_error("unsupported folding type");
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

    trained_ = true;
    return *this;
}

std::shared_ptr<Classifier> ClassifierCreatorFeatureSelection::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTransformedFeatures>(pC_,pFT_);
}

void ClassifierCreatorFeatureSelection::create_n_samples_weights() {
    std::default_random_engine generator;
    const auto pSample = get_sample();
    v_pSampleTrain_.resize(n_folds_);
    v_pSampleValidate_.resize(n_folds_);
    for (size_t i = 0; i < n_folds_; i++) {
        v_pSampleTrain_[i] = std::make_shared<Sample>(pSample->dim());
        v_pSampleValidate_[i] = std::make_shared<Sample>(pSample->dim());
    }
    for (size_t j = 0; j < pSample->size(); j++) {
        auto wn = (*pSample)[j]->get_weight_negatives();
        auto wp = (*pSample)[j]->get_weight_positives();
        std::vector<double> weights;
        generate_weights(weights, generator);
        for (size_t k = 0; k < n_folds_; k++) {
            if (weights[k] == 0) continue;
            std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>((*pSample)[j]->get_data());
            pFV->inc_weight_negatives(wn*weights[k]);
            pFV->inc_weight_positives(wp*weights[k]);
            v_pSampleValidate_[k]->push(pFV);
        }
    }

    for (size_t i = 0; i < n_folds_; i++) {
        for (size_t j = 0; j < n_folds_; j++) {
            if (i == j) continue;
            for (size_t k = 0; k < v_pSampleValidate_[j]->size(); k++) {
                std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>((*v_pSampleValidate_[j])[k]->get_data());
                auto wn = (*v_pSampleValidate_[j])[k]->get_weight_negatives();
                auto wp = (*v_pSampleValidate_[j])[k]->get_weight_positives();
                pFV->inc_weight_negatives(wn);
                pFV->inc_weight_positives(wp);
                v_pSampleTrain_[i]->push(pFV);
            }
        }
    }
}

void ClassifierCreatorFeatureSelection::create_n_samples_split() {
    std::default_random_engine generator;
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

void ClassifierCreatorFeatureSelection::generate_weights(std::vector<double> & v, std::default_random_engine & generator) const {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    v.resize(n_folds_);
    double sum = 0;
    for (size_t i = 0; i < n_folds_; i++) {
        v[i] = distribution(generator);
        sum += v[i];
    }
    for (size_t i = 0; i < n_folds_; i++) {
        v[i] /= sum;
    }
}

#endif