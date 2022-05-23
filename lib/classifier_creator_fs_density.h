#ifndef LIB_CLASSIFIER_CREATOR_FS_DENSITY_H_
#define LIB_CLASSIFIER_CREATOR_FS_DENSITY_H_

///  feature forward selection using auc vs density trade off

#include "classifier_creator_fs.h"
#include "evaluator.h"
#include "sample_creator.h"

class ClassifierCreatorFsDensity : public ClassifierCreatorFs {
public:
    ClassifierCreatorFsDensity();

    ClassifierCreatorFsDensity & init(const std::shared_ptr<Sample> &) override;

private:
    std::vector<std::shared_ptr<Sample>> v_pSampleTrain_;
    std::vector<std::shared_ptr<Sample>> v_pSampleValidate_;
    double best_target_kpi_;

    /// returns true if found a feature improving the performance
    bool check4additional_feature(const std::shared_ptr<FeatureMask> &);

    /// returns { {roc_train_err, roc_eval_err}, {classification_train_err, classification_eval_err} }
    [[nodiscard]] std::tuple<double, double, double, double, double, double, double, double>
    compute_kpi(const std::shared_ptr<const FeatureTransform> &) const;

    void select(const std::shared_ptr<FeatureMask> &) override;

    void reset() override;
};

ClassifierCreatorFsDensity::ClassifierCreatorFsDensity() : v_pSampleTrain_(0), v_pSampleValidate_(0),
best_target_kpi_(std::numeric_limits<double>::max()) {
}

ClassifierCreatorFsDensity &ClassifierCreatorFsDensity::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorFs::init(pSample);
    reset();
    return *this;
}

void ClassifierCreatorFsDensity::select(const std::shared_ptr<FeatureMask> & pFM) {
    best_target_kpi_ = std::numeric_limits<double>::max();
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");

    if (pFM->count() > 0) {
        if (verbose()) {
            std::cout << "Started feature selection with non-empty feature choice: ";
            const auto fm_pair = pFM->to_strings();
            std::cout << " feature_mask = \"" << fm_pair.first << "\", ";
            std::cout << "sign_mask = \"" << fm_pair.second << "\"" << std::endl;
        }
        auto pFT = std::make_shared<FeatureTransformSubset>(pFM);
        double A1, A2, B1, B2, C1, C2, D1, D2;
        std::tie(A1, A2, B1, B2, C1, C2, D1, D2) = compute_kpi(pFT);
        best_target_kpi_ = (A1 + B1 + C1 + D1 + A2 + B2 + C2 + D2) / 4;
    }
    while (check4additional_feature(pFM));
}

bool ClassifierCreatorFsDensity::check4additional_feature(const std::shared_ptr<FeatureMask> & pFM) {
    double A1, A2, B1, B2, C1, C2, D1, D2;
    double target_kpi;
    size_t best_feature_index = pFM->dim();
    bool best_sign = false;
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
            std::tie(A1, A2, B1, B2, C1, C2, D1, D2) = compute_kpi(pFT);
            target_kpi = (A1 + B1 + C1 + D1 + A2 + B2 + C2 + D2) / 4;
            if (target_kpi < best_target_kpi_) {
                best_target_kpi_ = target_kpi;
                best_feature_index = i;
                best_sign = sign;
            }
            (*pFM)[i] = false;
            pFM->sign(i) = false;
        }
    }
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

std::tuple<double, double, double, double, double, double, double, double> ClassifierCreatorFsDensity::
compute_kpi(const std::shared_ptr<const FeatureTransform> & pFT) const {
    Sample::WeightType A1 = 0, B1 = 0, C1 = 0, D1 = 0;
    Sample::WeightType A2 = 0, B2 = 0, C2 = 0, D2 = 0;
    const auto pSample = SampleCreator::transform_features(get_sample(),pFT);

    Sample::WeightType n,p;
    std::tie(n,p) = pSample->get_neg_pos_counts();
    std::cout << "n=" << n << " p=" << p << " n*p=" << n*p << " (n+p)*(n+p)=" << (n+p)*(n+p) << std::endl;
    if (n*p<0 || (n+p)*(n+p)<0) {
        std::cout << "n=" << n << " p=" << p << " n*p=" << n*p << " (n+p)*(n+p)=" << (n+p)*(n+p) << std::endl;
        throw std::runtime_error("Weight type is not sufficient");
    }
    for (size_t i = 0; i < pSample->size(); i++) {
        const auto & fv_i = *(*pSample)[i];
        for (size_t j = 0; j < pSample->size(); j++) {
            const auto & fv_j = *(*pSample)[j];
            if (fv_i <= fv_j) {
                Sample::WeightType f = 2;
                if (i == j) f = 1;
                A2 +=     fv_i.get_weight_positives() * fv_j.get_weight_negatives();
                B2 += f * fv_i.get_weight_positives() * fv_j.get_weight_positives();
                B2 += f * fv_i.get_weight_negatives() * fv_j.get_weight_negatives();
                C2 +=     fv_i.get_weight_negatives() * fv_j.get_weight_positives();
                D2 += f * (fv_i.get_weight_positives() + fv_i.get_weight_negatives())
                        * (fv_j.get_weight_positives() + fv_j.get_weight_negatives());
                if (fv_i < fv_j) {
                    A1 +=     fv_i.get_weight_positives() * fv_j.get_weight_negatives();
                    B1 += f * fv_i.get_weight_positives() * fv_j.get_weight_positives();
                    B1 += f * fv_i.get_weight_negatives() * fv_j.get_weight_negatives();
                    C1 +=     fv_i.get_weight_negatives() * fv_j.get_weight_positives();
                    D1 += f * (fv_i.get_weight_positives() + fv_i.get_weight_negatives())
                            * (fv_j.get_weight_positives() + fv_j.get_weight_negatives());
                }
                if (A1<0 || A2<0 || B1<0 || B2<0 || C1<0 || C2<0 || D1<0 || D2<0)
                    throw std::runtime_error("Weight type is not sufficient");
            }
        }
    }
    double A1_ = ((double)A1)/(double)(p*n);
    double A2_ = ((double)A2)/(double)(p*n);
    double B1_ = ((double)B1)/(double)(p*p + n*n);
    double B2_ = ((double)B2)/(double)(p*p + n*n);
    double C1_ = ((double)C1)/(double)(p*n);
    double C2_ = ((double)C2)/(double)(p*n);
    double D1_ = ((double)D1)/(double)((p+n)*(p+n));
    double D2_ = ((double)D2)/(double)((p+n)*(p+n));
    if (verbose()) {
        std::cout << "A1 = " << A1 << "/" << p*n << ", A2 = " << A2 << "/" << p*n;
        std::cout << ", B1 = " << B1 << "/" << p*p + n*n << ", B2 = " << B2 << "/" << p*p + n*n;
        std::cout << ", C1 = " << C1 << "/" << p*n << ", C2 = " << C2 << "/" << p*n;
        std::cout << ", D1 = " << D1 << "/" << (p+n)*(p+n) << ", D2 = " << D2 << "/" << (p+n)*(p+n);
        std::cout << std::endl;
    }
    return {A1_, A2_, B1_, B2_, C1_, C2_, D1_, D2_};
}

void ClassifierCreatorFsDensity::reset() {
    ClassifierCreatorFs::reset();
    v_pSampleTrain_.resize(0);
    v_pSampleValidate_.resize(0);
    best_target_kpi_ = std::numeric_limits<double>::max();
}

#endif