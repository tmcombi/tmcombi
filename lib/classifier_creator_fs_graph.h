#ifndef LIB_CLASSIFIER_CREATOR_FS_GRAPH_H_
#define LIB_CLASSIFIER_CREATOR_FS_GRAPH_H_

///  feature forward selection using graph optimization

#include "classifier_creator_train.h"
#include "classifier_transformed_features.h"
#include "feature_transform_subset.h"
#include "sample_creator.h"

#define CHECK_VS_SLOW_IMPL_

class ClassifierCreatorFsGraph : public ClassifierCreatorTrain {
public:

    ClassifierCreatorFsGraph();

    ClassifierCreatorFsGraph & init(const std::shared_ptr<Sample> &) override;
    ClassifierCreatorFsGraph & set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> &);
    
    /// with each further feature some of wrong and some of right binary relations are destroyed
    /// n_wrong = number of wrong binary relations that are destroyed
    /// n_right = number of right binary relations that are destroyed
    /// goal: n_wrong/n_right > threshold (default = 1) 
    ClassifierCreatorFsGraph & set_threshold_br(double);

    ClassifierCreatorFsGraph & train() override;

    std::shared_ptr<Classifier> get_classifier() const override;
private:
    std::shared_ptr<ClassifierCreatorTrain> pCCT_;
    std::shared_ptr<FeatureTransformSubset> pFT_;
    std::shared_ptr<Classifier> pC_;
    double threshold_br_;
    bool trained_;

#ifdef CHECK_VS_SLOW_IMPL
    static std::pair<double,double> compute_right_and_wrong_relations_from_sample_slow (const std::shared_ptr<Sample> &);
    static std::pair<double,double> compute_right_and_wrong_relations_from_reduced_sample (
            const std::shared_ptr<Sample> &,
            const boost::dynamic_bitset<> &,
            const boost::dynamic_bitset<> &
            );
    static std::pair<double,double> compute_right_and_wrong_delta_when_adding_feature_slow (
            const std::shared_ptr<Sample> &,
            const boost::dynamic_bitset<> &,
            const boost::dynamic_bitset<> &,
            size_t,
            bool
            );
#endif

    static void compute_relation_feature_wise(const std::shared_ptr<Sample> &,
            size_t, bool, std::vector<boost::dynamic_bitset<>> &);

    bool check4additional_feature(boost::dynamic_bitset<> &, boost::dynamic_bitset<> &,
                                  const std::vector<std::map<bool,std::vector<boost::dynamic_bitset<>>>> &,
                                          std::vector<boost::dynamic_bitset<>> &);

    static bool better_values(double, double, double, double);

    static std::pair<double,double> compute_right_and_wrong_delta_when_adding_feature (
            const std::shared_ptr<Sample> &,
            const std::vector<boost::dynamic_bitset<>> &,
            const std::vector<boost::dynamic_bitset<>> &
    );
};

ClassifierCreatorFsGraph::ClassifierCreatorFsGraph() :
pCCT_(nullptr), pFT_(nullptr), pC_(nullptr), threshold_br_(2), trained_(false) {
}

ClassifierCreatorFsGraph &ClassifierCreatorFsGraph::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    pFT_ = nullptr;
    pC_ = nullptr;
    trained_ = false;
    return *this;
}

ClassifierCreatorFsGraph &ClassifierCreatorFsGraph::
set_classifier_creator_train(const std::shared_ptr<ClassifierCreatorTrain> & pCCT) {
    if (pCCT_ != pCCT) {
        pCCT_ = pCCT;
        pFT_ = nullptr;
        pC_ = nullptr;
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

    const size_t size = pSample->size();
    const size_t dim = pSample->dim();

    /// dim * size * size; not larger relations feature-wise
    std::vector<std::map<bool,std::vector<boost::dynamic_bitset<>>>> relation(dim);
    const bool signs[] = {false, true};
    for (bool sign: signs) {
        for (size_t feature_index = 0; feature_index < dim; feature_index++) {
            compute_relation_feature_wise(pSample, feature_index, sign,relation[feature_index][sign]);
        }
    }

    std::vector<boost::dynamic_bitset<>> current_reachability(size);
    for (size_t i = 0; i < size; i++) {
        current_reachability[i].resize(size);
        current_reachability[i].flip();
    }

    boost::dynamic_bitset<> feature_mask(pSample->dim());
    boost::dynamic_bitset<> sign_mask(pSample->dim()); /// 0 for "+" and 1 for "-"
    while (check4additional_feature(feature_mask, sign_mask, relation, current_reachability));

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

#ifdef CHECK_VS_SLOW_IMPL
std::pair<double, double>
ClassifierCreatorFsGraph::compute_right_and_wrong_relations_from_sample_slow(const std::shared_ptr<Sample> & pSample) {
    double wrong = 0, right = 0;
    for (size_t i = 0; i < pSample->size(); i++) {
        const auto & fv_i = (*pSample)[i];
        for (size_t j = 0; j < pSample->size(); j++) {
            const auto & fv_j = (*pSample)[j];
            if ( *fv_i < *fv_j ) {
                //std::cout << *fv_i << " < " << *fv_j << " ->";
                const double wrong_contribution = fv_i->get_weight_positives()*fv_j->get_weight_negatives();
                const double right_contribution = fv_i->get_weight_negatives()*fv_j->get_weight_positives();
                //std::cout << " wrong contribution: " << wrong_contribution;
                //std::cout << ", right contribution: " << right_contribution << std::endl;
                wrong += wrong_contribution;
                right += right_contribution;
            } else {
                //std::cout << *fv_i << " not < " << *fv_j << std::endl;
            }
        }
    }

    return {wrong, right};
}

std::pair<double, double>
ClassifierCreatorFsGraph::compute_right_and_wrong_relations_from_reduced_sample(const std::shared_ptr<Sample> & pSample,
                                                                                const boost::dynamic_bitset<> & feature_mask,
                                                                                const boost::dynamic_bitset<> & sign_mask) {
    const auto pFT = std::make_shared<FeatureTransformSubset>(feature_mask, sign_mask);
    const auto pReducedSample = SampleCreator::transform_features(pSample,pFT);
    return compute_right_and_wrong_relations_from_sample_slow(pReducedSample);
}

std::pair<double, double>
ClassifierCreatorFsGraph::compute_right_and_wrong_delta_when_adding_feature_slow(const std::shared_ptr<Sample> & pSample,
                                                                                 const boost::dynamic_bitset<> & feature_mask,
                                                                                 const boost::dynamic_bitset<> & sign_mask,
                                                                                 size_t feature_indx,
                                                                                 bool sign) {
    const auto aux1 = compute_right_and_wrong_relations_from_reduced_sample(pSample,feature_mask,sign_mask);
    auto feature_mask2 = feature_mask;
    auto sign_mask2 = sign_mask;
    feature_mask2[feature_indx] = true;
    sign_mask2[feature_indx] = sign;
    const auto aux2 = compute_right_and_wrong_relations_from_reduced_sample(pSample,feature_mask2,sign_mask2);
    return {aux2.first-aux1.first,aux2.second-aux1.second};
}
#endif

void ClassifierCreatorFsGraph::compute_relation_feature_wise(const std::shared_ptr<Sample> & pSample,
                                                             const size_t feature_index,
                                                             const bool sign,
                                                             std::vector<boost::dynamic_bitset<>> & relation) {
    const auto size = pSample->size();
    relation.resize(size);
    const auto & fv2index_map = pSample->get_fv2index_map();
    const auto it_end = fv2index_map.end();
    std::multimap<double,size_t> value2index;
    for(auto it = fv2index_map.begin(); it != it_end; ++it) {
        if (sign) {
            value2index.insert(std::make_pair(-it->first[feature_index], it->second));
        } else {
            value2index.insert(std::make_pair(it->first[feature_index], it->second));
        }
    }
    if (value2index.empty())
        throw std::runtime_error("Empty multimap not expected");
    boost::dynamic_bitset<> current_reachability(size), next_reachability(size);
    current_reachability.flip(); next_reachability.flip();
    std::multimap<double,size_t>::const_iterator eq_range_begin = value2index.begin(), eq_range_end;
    for ( ; eq_range_begin != value2index.end(); eq_range_begin = eq_range_end ) {
        eq_range_end = value2index.upper_bound(eq_range_begin->first);
        for ( auto it = eq_range_begin; it != eq_range_end; ++it ) {
            next_reachability[it->second] = false;
            relation[it->second] = current_reachability;
        }
        current_reachability = next_reachability;
    }
    if (next_reachability.count() != 0) throw std::runtime_error("unexpected error");
}

bool ClassifierCreatorFsGraph::check4additional_feature(boost::dynamic_bitset<> & feature_mask,
        boost::dynamic_bitset<> & sign_mask,
        const std::vector<std::map<bool,std::vector<boost::dynamic_bitset<>>>> & relation,
        std::vector<boost::dynamic_bitset<>> & current_reachability) {
    double n_wrong_best = 1, n_right_best = threshold_br_;
    size_t best_feature_index = feature_mask.size();
    bool best_sign = false;
    for(size_t i=0; i < feature_mask.size(); i++) {
        if (feature_mask[i]) continue;
        const bool signs[] = {false, true};
        for (bool sign: signs) {
            double n_wrong_current, n_right_current;
            std::tie(n_wrong_current,n_right_current) =
                    compute_right_and_wrong_delta_when_adding_feature(get_sample(),current_reachability,relation[i].at(sign));
#ifdef CHECK_VS_SLOW_IMPL
            double n_wrong_slow, n_right_slow;
#define EPSILON_CLASSIFIER_CREATOR_FS_GRAPH 0.0000000001
            std::tie(n_wrong_slow, n_right_slow) =
                    compute_right_and_wrong_delta_when_adding_feature_slow(get_sample(),feature_mask,sign_mask,i,sign);
            const double rel_delta_wrong = abs(n_wrong_slow - n_wrong_current)/(abs(n_wrong_slow) + abs(n_wrong_current));
            const double rel_delta_right = abs(n_right_slow - n_right_current)/(abs(n_right_slow) + abs(n_right_current));
            //if (n_wrong_slow != n_wrong_current || n_right_slow != n_right_current) {
            if (rel_delta_wrong > EPSILON_CLASSIFIER_CREATOR_FS_GRAPH
            || rel_delta_right > EPSILON_CLASSIFIER_CREATOR_FS_GRAPH) {
                std::cout << "Slowly computed ";
                std::cout << " wrong relations change = \"" << n_wrong_slow << "\", ";
                std::cout << "right relations change = \"" << n_right_slow << "\"" << std::endl;
                std::cout << "Fast computed ";
                std::cout << " wrong relations change = \"" << n_wrong_current << "\", ";
                std::cout << "right relations change = \"" << n_right_current << "\"" << std::endl;
                std::cout << "Relative delta wrong relations change = " << abs(n_wrong_slow - n_wrong_current)/(abs(n_wrong_slow) + abs(n_wrong_current)) << std::endl;
                std::cout << "Relative delta right relations change = " << abs(n_right_slow - n_right_current)/(abs(n_right_slow) + abs(n_right_current)) << std::endl;
                throw std::runtime_error("unexpected error");
            }
#endif
            if (verbose()) {
                std::cout << std::endl;
                std::cout << "checking feature " << i << " with sign " << (sign ? "\"-\"" : "\"+\"") << ":";
                std::cout << " wrong relations change = \"" << n_wrong_current << "\", ";
                std::cout << "right relations change = \"" << n_right_current << "\"" << std::endl;
                std::cout << "comparing to currently best: ";
                std::cout << " wrong relations best change = \"" << n_wrong_best << "\", ";
                std::cout << "right relations best change = \"" << n_right_best << "\"" << std::endl;
            }

            if ( better_values(n_wrong_best, n_right_best, n_wrong_current, n_right_current) ) {
                std::cout << "accepting this feature temporarily" << std::endl;
                n_wrong_best = n_wrong_current;
                n_right_best = n_right_current;
                best_feature_index = i;
                best_sign = sign;
            }
        }
    }
    if ( best_feature_index < feature_mask.size() ) {
        feature_mask[best_feature_index] = true;
        sign_mask[best_feature_index] = best_sign;
        for(size_t i=0; i < get_sample()->size(); i++) {
            current_reachability[i] &= relation[best_feature_index].at(best_sign)[i];
        }
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

bool ClassifierCreatorFsGraph::better_values(const double n_wrong_best, const double n_right_best,
                                             const double n_wrong_current, const double n_right_current) {
    if (n_wrong_best >= 0 && n_right_best < 0)
        throw std::runtime_error("unexpected error");
    if (n_wrong_current >= 0 && n_right_current < 0)
        return false;
    if (n_wrong_best < 0 && n_right_best >= 0 && n_wrong_current < 0 && n_right_current >= 0)
        return n_right_current + n_wrong_current > n_right_best + n_wrong_best;
    if (n_wrong_current < 0 && n_right_current >= 0)
        return true;
    if (n_wrong_best < 0 && n_right_best >= 0)
        return false;
    if (n_wrong_best >= 0 && n_right_best >= 0 && n_wrong_current >= 0 && n_right_current >= 0)
        return n_right_current*n_wrong_best>n_wrong_current*n_right_best;
    if (n_wrong_best >= 0 && n_right_best >= 0 && n_wrong_current < 0 && n_right_current < 0)
        return (-n_wrong_current)*n_wrong_best > (-n_right_current)*n_right_best;
    if (n_wrong_best < 0 && n_right_best < 0 && n_wrong_current >= 0 && n_right_current >= 0)
        return n_wrong_current*(-n_wrong_best) < n_right_current*(-n_right_best);
    if (n_wrong_best < 0 && n_right_best < 0 && n_wrong_current < 0 && n_right_current < 0)
        return n_right_current*n_wrong_best<n_wrong_current*n_right_best;
    throw std::runtime_error("unexpected error");
}

std::pair<double, double> ClassifierCreatorFsGraph::
compute_right_and_wrong_delta_when_adding_feature(const std::shared_ptr<Sample> & pSample,
                                                  const std::vector<boost::dynamic_bitset<>> & current_reachability,
                                                  const std::vector<boost::dynamic_bitset<>> & relation) {
    const size_t size = pSample->size();
    if (size != current_reachability.size() || size != relation.size())
        throw std::runtime_error("unexpected error");
    double wrong_delta = 0, right_delta = 0;
    for (size_t i = 0; i < size; i++) {
        boost::dynamic_bitset<> lost_relations = relation[i];
        lost_relations.flip();
        lost_relations &= current_reachability[i];
        for (size_t j = lost_relations.find_first(); j < size; j = lost_relations.find_next(j)) {
            if (j==i) continue;
            if (current_reachability[j][i]) {
                wrong_delta += (*pSample)[i]->get_weight_negatives()*(*pSample)[j]->get_weight_positives();
                right_delta += (*pSample)[i]->get_weight_positives()*(*pSample)[j]->get_weight_negatives();
            } else {
                wrong_delta -= (*pSample)[i]->get_weight_positives()*(*pSample)[j]->get_weight_negatives();
                right_delta -= (*pSample)[i]->get_weight_negatives()*(*pSample)[j]->get_weight_positives();
            }
        }
    }
    return {wrong_delta,right_delta};
}


#endif