#ifndef LIB_CLASSIFIER_CREATOR_FS_AUC_H_
#define LIB_CLASSIFIER_CREATOR_FS_AUC_H_

///  feature forward selection using graph optimization

#include "classifier_creator_train.h"
#include "classifier_transformed_features.h"
#include "feature_transform_subset.h"
#include "sample_creator.h"

#define CHECK_VS_SLOW_IMPL

class ClassifierCreatorFsAuc : public ClassifierCreatorTrain {
public:

    ClassifierCreatorFsAuc();

    ClassifierCreatorFsAuc & init(const std::shared_ptr<Sample> &) override;
    ClassifierCreatorFsAuc & set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> &);

    ClassifierCreatorFsAuc & train() override;

    std::shared_ptr<Classifier> get_classifier() const override;
private:
    std::shared_ptr<ClassifierCreatorTrain> pCCT_;
    std::shared_ptr<FeatureTransformSubset> pFT_;
    std::shared_ptr<Classifier> pC_;
    double roc_err_;
    bool trained_;

#ifdef CHECK_VS_SLOW_IMPL
    static double roc_err_slow (const std::shared_ptr<Sample> &);
    static double roc_err_slow (
            const std::shared_ptr<Sample> &,
            const boost::dynamic_bitset<> &,
            const boost::dynamic_bitset<> &
            );
#endif

    bool check4additional_feature(boost::dynamic_bitset<> &, boost::dynamic_bitset<> &);

};

ClassifierCreatorFsAuc::ClassifierCreatorFsAuc() :
pCCT_(nullptr), pFT_(nullptr), pC_(nullptr), roc_err_(1), trained_(false) {
}

ClassifierCreatorFsAuc &ClassifierCreatorFsAuc::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    pFT_ = nullptr;
    pC_ = nullptr;
    roc_err_ = 1;
    trained_ = false;
    return *this;
}

ClassifierCreatorFsAuc &ClassifierCreatorFsAuc::
set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> & pCCT) {
    if (pCCT_ != pCCT) {
        pCCT_ = pCCT;
        pFT_ = nullptr;
        pC_ = nullptr;
        roc_err_ = 1;
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorFsAuc &ClassifierCreatorFsAuc::train() {
    if ( pCCT_ == nullptr ) throw std::runtime_error("run set_classifier_creator_train() prior to train");
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");
    if ( verbose() ) {
        double neg, pos;
        std::tie(neg,pos) = pSample->get_neg_pos_counts();
        std::cout << "Starting feature selection" << std::endl;
        std::cout << "Input Sample: size=" << pSample->size() << ", neg=" << neg << ", pos=" << pos;
        std::cout << ", roc_err=" << roc_err_slow(pSample) << std::endl;
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

std::shared_ptr<Classifier> ClassifierCreatorFsAuc::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTransformedFeatures>(pC_,pFT_);
}

#ifdef CHECK_VS_SLOW_IMPL
double ClassifierCreatorFsAuc::roc_err_slow(const std::shared_ptr<Sample> & pSample) {
    double conflicts = 0,n,p;
    std::tie(n,p) = pSample->get_neg_pos_counts();
    const double possible_conflicts = n*p;
    //std::cout << "pos=" << p << ", neg=" << n << ", possible conflicts=" << possible_conflicts << std::endl;
    for (size_t i = 0; i < pSample->size(); i++) {
        const auto & fv_i = *(*pSample)[i];
        for (size_t j = i; j < pSample->size(); j++) {
            const auto & fv_j = *(*pSample)[j];
            //std::cout << "fv["<< i << "] = " << fv_i << ", fv["<< j << "] = " << fv_j;
            if (i == j) {
                conflicts += fv_i.get_weight_positives() * fv_i.get_weight_negatives() / 2;
                //std::cout << ", contribution = " << fv_i.get_weight_positives() * fv_i.get_weight_negatives() / 2;
            } else if (fv_i < fv_j) {
                conflicts += fv_i.get_weight_positives() * fv_j.get_weight_negatives();
                //std::cout << ", contribution = " << fv_i.get_weight_positives() * fv_j.get_weight_negatives();
            } else if (fv_j < fv_i) {
                conflicts += fv_i.get_weight_negatives() * fv_j.get_weight_positives();
                //std::cout << ", contribution = " <<  fv_i.get_weight_negatives() * fv_j.get_weight_positives();
            } else {
                conflicts += fv_i.get_weight_positives() * fv_j.get_weight_negatives()/2;
                conflicts += fv_i.get_weight_negatives() * fv_j.get_weight_positives()/2;
                //std::cout << ", contribution = " <<  fv_i.get_weight_negatives() * fv_j.get_weight_positives()/2+
                //        fv_i.get_weight_positives() * fv_j.get_weight_negatives()/2;
            }
            //std::cout<< std::endl;
        }
    }
    //std::cout << "total conflicts=" << conflicts << ", roc_err=" << conflicts/possible_conflicts << std::endl;
    return conflicts/possible_conflicts;
}

double ClassifierCreatorFsAuc::roc_err_slow(const std::shared_ptr<Sample> & pSample,
                                     const boost::dynamic_bitset<> & feature_mask,
                                     const boost::dynamic_bitset<> & sign_mask) {
    const auto pFT = std::make_shared<FeatureTransformSubset>(feature_mask, sign_mask);
    const auto pReducedSample = SampleCreator::transform_features(pSample,pFT);
    return roc_err_slow(pReducedSample);
}
#endif

bool ClassifierCreatorFsAuc::check4additional_feature(boost::dynamic_bitset<> & feature_mask,
        boost::dynamic_bitset<> & sign_mask) {
    size_t best_feature_index = feature_mask.size();
    bool best_sign = false;
    for(size_t i=0; i < feature_mask.size(); i++) {
        if (feature_mask[i]) continue;
        const bool signs[] = {false, true};
        for (bool sign: signs) {
            feature_mask[i] = true;
            sign_mask[i] = sign;
            double roc_err_current = roc_err_slow(get_sample(),feature_mask,sign_mask);
            feature_mask[i] = false;
            sign_mask[i] = false;
            if (verbose()) {
                std::cout << std::endl;
                std::cout << "checking feature " << i << " with sign " << (sign ? "\"-\"" : "\"+\"") << ":";
                std::cout << " roc_err = \"" << roc_err_current << "\", ";
                std::cout << "comparing to currently best: ";
                std::cout << roc_err_ << "\"" << std::endl;
            }

            if ( roc_err_current < roc_err_ ) {
                std::cout << "accepting this feature temporarily" << std::endl;
                roc_err_ = roc_err_current;
                best_feature_index = i;
                best_sign = sign;
            }
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


#endif