#ifndef LIB_CLASSIFIER_CREATOR_FS_GRAPH_H_
#define LIB_CLASSIFIER_CREATOR_FS_GRAPH_H_

///  feature forward selection using graph optimization

#include "classifier_creator_train.h"
#include "classifier_transformed_features.h"
#include "feature_transform_subset.h"
#include "sample_creator.h"


class ClassifierCreatorFsGraph : public ClassifierCreatorTrain {
public:

    ClassifierCreatorFsGraph();

    ClassifierCreatorFsGraph & init(const std::shared_ptr<Sample> &) override;
    ClassifierCreatorFsGraph & set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> &);
    
    /// with each further feature some of wrong and some of correct binary relations are emerging or being destroyed
    /// delta_wrong = change of wrong binary relations
    /// delta_correct = change of correct binary relations
    /// goal: delta_correct/delta_wrong > threshold (default = 1) 
    ClassifierCreatorFsGraph & set_threshold_br(double);

    ClassifierCreatorFsGraph & train() override;

    std::shared_ptr<Classifier> get_classifier() const override;
private:
    std::shared_ptr<ClassifierCreatorTrain> pCCT_;
    std::shared_ptr<FeatureTransformSubset> pFT_;
    std::shared_ptr<Classifier> pC_;
    double threshold_br_;
    double n_wrong_best_;
    double n_correct_best_;
    bool trained_;

    bool check4additional_feature(boost::dynamic_bitset<> &, boost::dynamic_bitset<> &);

    static bool better_values(double, double, double, double);

    static std::pair<double,double> compute_correct_and_wrong_from_sample (const std::shared_ptr<Sample> &);
    static std::pair<double,double> compute_correct_and_wrong_from_reduced_sample (
            const std::shared_ptr<Sample> &,
            const boost::dynamic_bitset<> &,
            const boost::dynamic_bitset<> &
    );

};

ClassifierCreatorFsGraph::ClassifierCreatorFsGraph() : pCCT_(nullptr), pFT_(nullptr), pC_(nullptr), threshold_br_(2),
n_wrong_best_(0), n_correct_best_(0), trained_(false) {
}

ClassifierCreatorFsGraph &ClassifierCreatorFsGraph::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    pFT_ = nullptr;
    pC_ = nullptr;
    n_wrong_best_ = 0;
    n_correct_best_ = 0;
    trained_ = false;
    return *this;
}

ClassifierCreatorFsGraph &ClassifierCreatorFsGraph::
set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> & pCCT) {
    if (pCCT_ != pCCT) {
        pCCT_ = pCCT;
        pFT_ = nullptr;
        pC_ = nullptr;
        n_wrong_best_ = 0;
        n_correct_best_ = 0;
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorFsGraph &ClassifierCreatorFsGraph::set_threshold_br(const double threshold_br) {
    if (threshold_br_ != threshold_br) {
        if (threshold_br < 0) throw std::runtime_error("threshold_br must be positive");
        threshold_br_ = threshold_br;
        pFT_ = nullptr;
        pC_ = nullptr;
        n_wrong_best_ = 0;
        n_correct_best_ = 0;
        trained_ = false;
    }
    return *this;
}

ClassifierCreatorFsGraph &ClassifierCreatorFsGraph::train() {
    if ( pCCT_ == nullptr ) throw std::runtime_error("run set_classifier_creator_train() prior to train");
    const auto pSample = get_sample();
    if ( pSample == nullptr ) throw std::runtime_error("specify sample prior training");
    if ( verbose() ) {
        double neg, pos;
        std::tie(neg,pos) = pSample->get_neg_pos_counts();
        std::cout << "Starting feature selection" << std::endl;
        std::cout << "Input Sample: size=" << pSample->size() << ", neg=" << neg << ", pos=" << pos << std::endl;
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

std::shared_ptr<Classifier> ClassifierCreatorFsGraph::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTransformedFeatures>(pC_,pFT_);
}

std::pair<double, double>
ClassifierCreatorFsGraph::compute_correct_and_wrong_from_reduced_sample(const std::shared_ptr<Sample> & pSample,
                                                                           const boost::dynamic_bitset<> & feature_mask,
                                                                           const boost::dynamic_bitset<> & sign_mask) {
    const auto pFT = std::make_shared<FeatureTransformSubset>(feature_mask, sign_mask);
    const auto pReducedSample = SampleCreator::transform_features(pSample,pFT);
    return compute_correct_and_wrong_from_sample(pReducedSample);
}

std::pair<double, double>
ClassifierCreatorFsGraph::compute_correct_and_wrong_from_sample(const std::shared_ptr<Sample> & pSample) {

    const size_t size = pSample->size();
    const size_t dim = pSample->dim();
    std::vector<boost::dynamic_bitset<>> adjacency_matrix(size);
    for(size_t i = 0; i < size; i++) {
        adjacency_matrix[i].resize(size,true);
    }
    const auto & fv2index_map = pSample->get_fv2index_map();
    const auto it_end = fv2index_map.end();
    for (size_t feature_index = 0; feature_index < dim; feature_index++) {
        std::multimap<double,size_t> value2index;
        for(auto it = fv2index_map.begin(); it != it_end; ++it) {
            value2index.insert(std::make_pair(it->first[feature_index], it->second));
        }
        if (value2index.empty())
            throw std::runtime_error("Empty multimap not expected");
        boost::dynamic_bitset<> current_reachability(size), nodes2visit(size);
        current_reachability.flip(); nodes2visit.flip();
        std::multimap<double,size_t>::const_iterator eq_range_begin = value2index.begin(), eq_range_end;
        for ( ; eq_range_begin != value2index.end(); eq_range_begin = eq_range_end ) {
            eq_range_end = value2index.upper_bound(eq_range_begin->first);
            for ( auto it = eq_range_begin; it != eq_range_end; ++it ) {
                current_reachability[it->second] = false;
                adjacency_matrix[it->second] &= current_reachability;
                current_reachability[it->second] = true;
                nodes2visit[it->second] = false;
            }
            current_reachability &= nodes2visit;
        }
    }

    double wrong = 0, correct = 0;
    for(size_t i = 0; i < size; i++) {
        const auto & fv_i = *(*pSample)[i];
        for(size_t j = adjacency_matrix[i].find_first(); j < size; j = adjacency_matrix[i].find_next(j)) {
            const auto & fv_j = *(*pSample)[j];
            const double wrong_contribution = fv_i.get_weight_positives()*fv_j.get_weight_negatives();
            const double correct_contribution = fv_i.get_weight_negatives()*fv_j.get_weight_positives();
            wrong += wrong_contribution;
            correct += correct_contribution;
        }
    }
    return {wrong, correct};
}

bool ClassifierCreatorFsGraph::check4additional_feature(boost::dynamic_bitset<> & feature_mask,
        boost::dynamic_bitset<> & sign_mask) {
    double delta_wrong_best = 1, delta_correct_best = threshold_br_, n_wrong_best = 0, n_correct_best = 0;
    bool best_sign = false;
    size_t best_feature_index = feature_mask.size();
    for(size_t i=0; i < feature_mask.size(); i++) {
        if (feature_mask[i]) continue;
        const bool signs[] = {false, true};
        for (bool sign: signs) {
            double n_wrong_current, n_correct_current;
            feature_mask[i] = true;
            sign_mask[i] = sign;
            std::tie(n_wrong_current,n_correct_current) =
                    compute_correct_and_wrong_from_reduced_sample(get_sample(),feature_mask,sign_mask);
            feature_mask[i] = false;
            sign_mask[i] = false;
            const double delta_wrong_current = n_wrong_current - n_wrong_best_;
            const double delta_correct_current = n_correct_current - n_correct_best_;
            if (verbose()) {
                std::cout << std::endl;
                std::cout << "checking feature " << i << " with sign " << (sign ? "\"-\"" : "\"+\"") << ":";
                std::cout << " wrong relations change = \"" << delta_wrong_current << "\", ";
                std::cout << "correct relations change = \"" << delta_correct_current << "\"" << std::endl;
                std::cout << "comparing to currently best: ";
                std::cout << " wrong relations best change = \"" << delta_wrong_best << "\", ";
                std::cout << "correct relations best change = \"" << delta_correct_best << "\"" << std::endl;
            }

            if ( better_values(delta_wrong_best, delta_correct_best, delta_wrong_current, delta_correct_current) ) {
                std::cout << "accepting this feature temporarily" << std::endl;
                n_wrong_best = n_wrong_current;
                n_correct_best = n_correct_current;
                delta_wrong_best = delta_wrong_current;
                delta_correct_best = delta_correct_current;
                best_feature_index = i;
                best_sign = sign;
            }
        }
    }
    if ( best_feature_index < feature_mask.size() ) {
        feature_mask[best_feature_index] = true;
        sign_mask[best_feature_index] = best_sign;
        n_wrong_best_ = n_wrong_best;
        n_correct_best_ = n_correct_best;
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

bool ClassifierCreatorFsGraph::better_values(const double n_wrong_best, const double n_correct_best,
                                             const double n_wrong_current, const double n_correct_current) {
    if (n_wrong_best >= 0 && n_correct_best < 0)
        throw std::runtime_error("unexpected error");
    if (n_wrong_current >= 0 && n_correct_current < 0)
        return false;
    if (n_wrong_best < 0 && n_correct_best >= 0 && n_wrong_current < 0 && n_correct_current >= 0)
        return n_correct_current + n_wrong_current > n_correct_best + n_wrong_best;
    if (n_wrong_current < 0 && n_correct_current >= 0)
        return true;
    if (n_wrong_best < 0 && n_correct_best >= 0)
        return false;
    if (n_wrong_best >= 0 && n_correct_best >= 0 && n_wrong_current >= 0 && n_correct_current >= 0)
        return n_correct_current*n_wrong_best>n_wrong_current*n_correct_best;
    if (n_wrong_best >= 0 && n_correct_best >= 0 && n_wrong_current < 0 && n_correct_current < 0)
        return (-n_wrong_current)*n_wrong_best > (-n_correct_current)*n_correct_best;
    if (n_wrong_best < 0 && n_correct_best < 0 && n_wrong_current >= 0 && n_correct_current >= 0)
        return n_wrong_current*(-n_wrong_best) < n_correct_current*(-n_correct_best);
    if (n_wrong_best < 0 && n_correct_best < 0 && n_wrong_current < 0 && n_correct_current < 0)
        return n_correct_current*n_wrong_best<n_wrong_current*n_correct_best;
    throw std::runtime_error("unexpected error");
}

#endif