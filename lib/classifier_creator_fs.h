#ifndef LIB_CLASSIFIER_CREATOR_FS_H_
#define LIB_CLASSIFIER_CREATOR_FS_H_

#include "classifier_creator_superposition.h"
#include "classifier_transformed_features.h"
#include "feature_selection.h"
#include "feature_transform_subset.h"
#include "sample_creator.h"


/// feature selection - base class

class ClassifierCreatorFs : public ClassifierCreatorSuperposition, public FeatureSelection {
public:
    ClassifierCreatorFs();

    ClassifierCreatorFs & init(const std::shared_ptr<Sample> &) override;

    std::shared_ptr<Classifier> get_classifier() const override;

    std::shared_ptr<Classifier> get_classifier_on_transformed_features() const;

    ClassifierCreatorFs & train() override;

protected:
    void reset() override;
    virtual void select(const std::shared_ptr<FeatureMask> &) = 0;
    std::pair<size_t,bool> analyze_current_mask(const std::shared_ptr<FeatureMask> &) const;

private:
    std::shared_ptr<FeatureTransformSubset> pFT_;
    std::shared_ptr<Classifier> pC_;
    bool trained_;

    /// 0 if equal, -1 if v1 < v2, 1 if v1 > v2, -2 otherwise
    static int compare_wrt_FM(const FeatureVector &, const FeatureVector &, const FeatureMask &);
};

ClassifierCreatorFs::ClassifierCreatorFs() : pFT_(nullptr), pC_(nullptr), trained_(false) {
}

ClassifierCreatorFs &ClassifierCreatorFs::init(const std::shared_ptr<Sample> & pSample) {
    pFM_ = nullptr;
    ClassifierCreatorTrain::init(pSample);
    reset();
    return *this;
}

std::shared_ptr<Classifier> ClassifierCreatorFs::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTransformedFeatures>(pC_,pFT_);
}

std::shared_ptr<Classifier> ClassifierCreatorFs::get_classifier_on_transformed_features() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return pC_;
}

ClassifierCreatorFs &ClassifierCreatorFs::train() {
    if ( pCCT_ == nullptr ) throw std::runtime_error("run set_classifier_creator_train() prior to train");
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");
    if ( verbose() ) {
        std::cout << std::endl;
        std::cout << "Starting feature selection" << std::endl;
        double neg, pos;
        std::tie(neg,pos) = pSample->get_neg_pos_counts();
        std::cout << "Input Sample: size=" << pSample->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
    }

    if (pFM_ == nullptr)
        pFM_ = std::make_shared<FeatureMask>(pSample->dim());
    if (pFM_->dim() != pSample->dim())
        throw std::runtime_error("Dimensions of FeatureMask and Sample have to be equal");
    this->select(pFM_);

    if (verbose()) {
        const auto fm_pair = pFM_->to_strings();
        std::cout << std::endl;
        std::cout << "###################################################################################" << std::endl;
        std::cout << "Feature selection finished: ";
        std::cout << "feature_mask = \"" << fm_pair.first << "\", ";
        std::cout << "sign_mask = \"" << fm_pair.second << "\"" << std::endl;
        std::cout << "Training now the classifier with the selected features on the whole sample";
        std::cout << std::endl;
    }

    pFT_ = std::make_shared<FeatureTransformSubset>(pFM_);
    const auto pSampleTransformed = SampleCreator::transform_features(pSample, pFT_);
    (*pCCT_).init(pSampleTransformed).train();
    pC_ = pCCT_->get_classifier();

    trained_ = true;
    return *this;
}

void ClassifierCreatorFs::reset() {
    pFT_ = nullptr;
    pC_ = nullptr;
    trained_ = false;
}

std::pair<size_t,bool> ClassifierCreatorFs::analyze_current_mask(const std::shared_ptr<FeatureMask> & pFM) const {
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");
    // sign "+" / "-"
    std::vector<std::pair<double,double>>
        equal2right(pFM->dim(),{0,0}),
        equal2wrong(pFM->dim(),{0,0}),
        right2incomparable(pFM->dim(),{0,0}),
        wrong2incomparable(pFM->dim(),{0,0});
    for (size_t i = 0; i < pSample->size(); i++) {
        const auto & fv1 = *(*pSample)[i];
        for (size_t j = i+1; j < pSample->size(); j++) {
            const auto & fv2 = *(*pSample)[j];
            const auto cmp = compare_wrt_FM(fv1, fv2, *pFM);
            if (cmp==-2) continue;
            for (size_t k = 0; k < pFM->dim(); k++) {
                if ((*pFM)[k]) continue;
                if (cmp == 0) {
                    if (fv1[k] < fv2[k]) {
                        equal2right[k].first += fv1.get_weight_negatives()*fv2.get_weight_positives();
                        equal2right[k].second += fv2.get_weight_negatives()*fv1.get_weight_positives();
                        equal2wrong[k].first += fv1.get_weight_positives()*fv2.get_weight_negatives();
                        equal2wrong[k].second += fv2.get_weight_positives()*fv1.get_weight_negatives();
                    }
                    if (fv1[k] > fv2[k]) {
                        equal2right[k].first += fv1.get_weight_positives()*fv2.get_weight_negatives();
                        equal2right[k].second += fv2.get_weight_positives()*fv1.get_weight_negatives();
                        equal2wrong[k].first += fv1.get_weight_negatives()*fv2.get_weight_positives();
                        equal2wrong[k].second += fv2.get_weight_negatives()*fv1.get_weight_positives();
                    }
                }
                if (cmp == -1 && fv1[k] > fv2[k]) {
                    right2incomparable[k].first += fv1.get_weight_negatives()*fv2.get_weight_positives();
                    wrong2incomparable[k].first += fv1.get_weight_positives()*fv2.get_weight_negatives();
                }
                if (cmp == -1 && fv1[k] < fv2[k]) {
                    right2incomparable[k].second += fv1.get_weight_negatives()*fv2.get_weight_positives();
                    wrong2incomparable[k].second += fv1.get_weight_positives()*fv2.get_weight_negatives();
                }
                if (cmp == 1 && fv1[k] < fv2[k]) {
                    right2incomparable[k].first += fv2.get_weight_negatives()*fv1.get_weight_positives();
                    wrong2incomparable[k].first += fv2.get_weight_positives()*fv1.get_weight_negatives();
                }
                if (cmp == 1 && fv1[k] > fv2[k]) {
                    right2incomparable[k].second += fv2.get_weight_negatives()*fv1.get_weight_positives();
                    wrong2incomparable[k].second += fv2.get_weight_positives()*fv1.get_weight_negatives();
                }
            }
        }
    }
    size_t best_feature = pFM->dim();
    bool best_sign = false;
    double best_ratio = 0.15;
    std::cout << std::endl;
    std::cout << "feature/sign\tequal2right\tequal2wrong\tright2incomp\twrong2incomp\teq_ratio\tincomp_ratio\tarithm_mittel\tgeom_mittel\tratio" << std::endl;
    for (size_t k = 0; k < pFM->dim(); k++) {
        const auto equal_change_first = equal2right[k].first/equal2wrong[k].first;
        const auto incomparable_change_first = wrong2incomparable[k].first/right2incomparable[k].first;
        const auto arithm_mittel_first = (equal_change_first+incomparable_change_first)/2;
        const auto geom_mittel_first = sqrt(equal_change_first*incomparable_change_first);
        const auto ratio_first = (equal2right[k].first+wrong2incomparable[k].first)/(equal2wrong[k].first+right2incomparable[k].first);
        std::cout << "     " << k << "/+\t" << equal2right[k].first << "\t" << equal2wrong[k].first << "\t";
        std::cout << right2incomparable[k].first << "\t" << wrong2incomparable[k].first;
        std::cout << "\t" << equal_change_first;
        std::cout << "  \t" << incomparable_change_first;
        std::cout << "  \t" << arithm_mittel_first;
        std::cout << "  \t" << geom_mittel_first;
        std::cout << "  \t" << ratio_first;
        std::cout << std::endl;
        if ((equal2wrong[k].first+right2incomparable[k].first == 0 && equal2right[k].first+wrong2incomparable[k].first > 0)
        || (equal2wrong[k].first+right2incomparable[k].first > 0 && ratio_first >= best_ratio)) {
            best_ratio = ratio_first;
            best_feature = k;
            best_sign = false;
        }

        const auto equal_change_second = equal2right[k].second/equal2wrong[k].second;
        const auto incomparable_change_second = wrong2incomparable[k].second/right2incomparable[k].second;
        const auto arithm_mittel_second = (equal_change_second+incomparable_change_second)/2;
        const auto geom_mittel_second = sqrt(equal_change_second*incomparable_change_second);
        const auto ratio_second = (equal2right[k].second+wrong2incomparable[k].second)/(equal2wrong[k].second+right2incomparable[k].second);
        std::cout << "     " << k << "/-\t" << equal2right[k].second << "\t" << equal2wrong[k].second << "\t";
        std::cout << right2incomparable[k].second << "\t" << wrong2incomparable[k].second;
        std::cout << "\t" << equal_change_second;
        std::cout << "  \t" << incomparable_change_second;
        std::cout << "  \t" << arithm_mittel_second;
        std::cout << "  \t" << geom_mittel_second;
        std::cout << "  \t" << ratio_second;
        std::cout << std::endl;
        if ((equal2wrong[k].second+right2incomparable[k].second == 0 && equal2right[k].second+wrong2incomparable[k].second > 0)
            || (equal2wrong[k].second+right2incomparable[k].second > 0 && ratio_second >= best_ratio)) {
            best_ratio = ratio_second;
            best_feature = k;
            best_sign = true;
        }

    }
    std::cout << "Recommend feature " << best_feature << " with sign " << best_sign;
    std::cout << std::endl;
    return {best_feature, best_sign};
}

/// 0 if equal, -1 if v1 < v2, 1 if v1 > v2, -2 otherwise
int ClassifierCreatorFs::compare_wrt_FM(const FeatureVector & fv1, const FeatureVector & fv2, const FeatureMask & fm) {
    bool smaller_found = false, larger_found = false;
    for (size_t i = fm.find_first(); i < fm.dim(); i = fm.find_next(i) ) {
        if (smaller_found && larger_found) break;
        if (fv1[i] < fv2[i]) {
            smaller_found = true;
        }
        if (fv1[i] > fv2[i]) {
            larger_found = true;
        }
    }
    if (!smaller_found && !larger_found) return 0;
    if ( smaller_found && !larger_found) return -1;
    if (!smaller_found &&  larger_found) return  1;
    return -2;
}

#endif