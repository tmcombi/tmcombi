#ifndef LIB_SAMPLE_CREATOR_H_
#define LIB_SAMPLE_CREATOR_H_

#include <random>

#include "feature_names.h"
#include "border.h"
#include "sample.h"
#include "feature_transform.h"

class SampleCreator {
public:
    SampleCreator();

    void set_feature_names(std::shared_ptr<FeatureNames>);

    std::shared_ptr<Sample> from_stream(std::istream &);
    std::shared_ptr<Sample> from_file(const std::string &);

    static std::shared_ptr<Sample> merge(const std::shared_ptr<Sample> &, const std::shared_ptr<Sample> &);

    static std::shared_ptr<Border> lower_border(const std::shared_ptr<Border>&, const std::shared_ptr<Border>&);
    static std::shared_ptr<Border> upper_border(const std::shared_ptr<Border>&, const std::shared_ptr<Border>&);

    /// prefer using graph if available
    template <typename GraphType>
    static std::shared_ptr<Border> lower_border(const std::shared_ptr<Sample>&, const std::shared_ptr<GraphType>&);
    template <typename GraphType>
    static std::shared_ptr<Border> upper_border(const std::shared_ptr<Sample>&, const std::shared_ptr<GraphType>&);

    static std::pair< std::shared_ptr<Sample>, std::shared_ptr<Sample> >
    split_sample(const std::shared_ptr<Sample>&, const boost::dynamic_bitset<> &);

    /// transform features and create induced sample
    static std::shared_ptr<Sample> transform_features(
            const std::shared_ptr<const Sample> &,
            const std::shared_ptr<const FeatureTransform> &);

    /// randomly splits sample into n samples based on seed, output vector is filled with subsamples
    static void create_n_samples_split(const std::shared_ptr<const Sample> &, size_t,
            unsigned long, std::vector<std::shared_ptr<Sample>> &);

private:
    std::shared_ptr<FeatureNames> pFN_;
};

SampleCreator::SampleCreator() : pFN_(nullptr){
}

void SampleCreator::set_feature_names(std::shared_ptr<FeatureNames> pFN) {
    pFN_ = std::move(pFN);
}

std::shared_ptr<Sample> SampleCreator::from_stream(std::istream & is) {
    if (!pFN_) throw std::domain_error("You have to set feature_names before importing from stream!");

    DataContainer<double> dataContainer(pFN_->get_feature_indices().size());
    std::string line;
    while (std::getline(is, line)) {
        line = std::regex_replace (line,std::regex("\r$"),"");
        const auto pFV =
                std::make_shared<FeatureVectorTemplated<double>>(line,
                                                pFN_->get_feature_indices(),
                                                pFN_->get_target_feature_index(),
                                                pFN_->get_negatives_label(),
                                                pFN_->get_positives_label(),
                                                pFN_->get_weight_index());
        dataContainer.push(pFV);
    }

    auto pSample = std::make_shared<Sample>(pFN_->get_feature_indices().size());
    for (size_t i = 0; i < dataContainer.size(); i++) {
        pSample->push(dataContainer[i]);
    }
    return pSample;
}

std::shared_ptr<Sample> SampleCreator::from_file(const std::string & data_file) {
#ifdef TIMERS
    const std::clock_t time1 = std::clock();
#endif
    std::ifstream fs_data(data_file);
    if (!fs_data.is_open())
        throw std::runtime_error("Cannot open file: " + data_file);
    std::shared_ptr<Sample> pSample = from_stream(fs_data);
    fs_data.close();
#ifdef TIMERS
    const std::clock_t time2 = std::clock();
    std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Reading data from file" << std::endl;
#endif
    return pSample;
}

std::shared_ptr<Sample> SampleCreator::merge(const std::shared_ptr<Sample> & pDC1,
                                             const std::shared_ptr<Sample> & pDC2) {
    if (pDC1->dim() != pDC2->dim())
        throw std::domain_error("Unexpected error: trying to merge samples of different dimensions!");
    std::shared_ptr<Sample> pSample = std::make_shared<Sample>(pDC1->dim());
    for (size_t i = 0; i < pDC1->size(); ++i)
        pSample->push((*pDC1)[i]);
    for (size_t j = 0; j < pDC2->size(); ++j)
        pSample->push((*pDC2)[j]);
    return pSample;
}

std::shared_ptr<Border> SampleCreator::
lower_border(const std::shared_ptr<Border> & pAboveBorder, const std::shared_ptr<Border> & pBelowBorder) {
    if (pAboveBorder->dim() != pBelowBorder->dim())
        throw std::runtime_error("Dimensions of the compared borders do not coincide");
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pBelowBorder->dim());
    for (size_t i=0; i < pBelowBorder->size(); i++) {
        pBorder->push((*pBelowBorder)[i]);
    }
    for (size_t i=0; i < pAboveBorder->size(); i++) {
        if ( !pBelowBorder->point_above((*pAboveBorder)[i]->get_data()) )
            pBorder->push((*pAboveBorder)[i]);
    }
    return pBorder;
}

std::shared_ptr<Border> SampleCreator::
upper_border(const std::shared_ptr<Border> & pBelowBorder, const std::shared_ptr<Border> & pAboveBorder) {
    if (pAboveBorder->dim() != pBelowBorder->dim())
        throw std::runtime_error("Dimensions of the compared borders do not coincide");
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pBelowBorder->dim());
    for (size_t i=0; i < pAboveBorder->size(); i++) {
        pBorder->push((*pAboveBorder)[i]);
    }
    for (size_t i=0; i < pBelowBorder->size(); i++) {
        if ( !pAboveBorder->point_below((*pBelowBorder)[i]->get_data()) )
            pBorder->push((*pBelowBorder)[i]);
    }
    return pBorder;
}

template <typename GraphType>
std::shared_ptr<Border> SampleCreator::
lower_border(const std::shared_ptr<Sample> & pSample, const std::shared_ptr<GraphType> & pGraph) {
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pSample->dim());
    const size_t size = pSample->size();
    const boost::dynamic_bitset<> & bits = pSample->get_lower_border(pGraph);
    for (size_t i = 0; i < size; ++i) {
        if (bits[i]) pBorder->push((*pSample)[i]);
    }
    pBorder->set_neg_pos_counts(pSample->get_neg_pos_counts());
    return pBorder;
}

template <typename GraphType>
std::shared_ptr<Border> SampleCreator::
upper_border(const std::shared_ptr<Sample> & pSample, const std::shared_ptr<GraphType> & pGraph) {
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pSample->dim());
    const size_t size = pSample->size();
    const boost::dynamic_bitset<> & bits = pSample->get_upper_border(pGraph);
    for (size_t i = 0; i < size; ++i) {
        if (bits[i]) pBorder->push((*pSample)[i]);
    }
    pBorder->set_neg_pos_counts(pSample->get_neg_pos_counts());
    return pBorder;
}

std::pair<std::shared_ptr<Sample>, std::shared_ptr<Sample> >
        SampleCreator::split_sample(const std::shared_ptr<Sample> & pSample, const boost::dynamic_bitset<> & db) {
    if (pSample->size() != db.size())
        throw std::domain_error("Sample and bitset have to be of the same size!");

    std::shared_ptr<Sample> pSampleLower = std::make_shared<Sample>(pSample->dim());
    std::shared_ptr<Sample> pSampleUpper = std::make_shared<Sample>(pSample->dim());

    for (size_t i = 0; i < pSample->size(); ++i) {
        if (db[i]) pSampleUpper->push((*pSample)[i]);
        else pSampleLower->push((*pSample)[i]);
    }
    return {pSampleLower, pSampleUpper};
}

std::shared_ptr<Sample> SampleCreator::transform_features(
        const std::shared_ptr<const Sample> & pSample,
        const std::shared_ptr<const FeatureTransform> & pFT) {
    auto pSampleTransformed = std::make_shared<Sample>(pFT->dim_out());
    const size_t size = pSample->size();
    for (size_t i = 0; i < size; i++) {
        const auto pFV = pFT->transform_feature_vector((*pSample)[i]);
        pSampleTransformed->push(pFV);
    }
    return pSampleTransformed;
}

void SampleCreator::create_n_samples_split(const std::shared_ptr<const Sample> & pSample, size_t n_folds,
                                           unsigned long seed, std::vector<std::shared_ptr<Sample>> & v_pSample) {
    std::default_random_engine generator(seed);
    v_pSample.resize(n_folds);
    for (size_t i = 0; i < n_folds; i++) {
        v_pSample[i] = std::make_shared<Sample>(pSample->dim());
    }
    std::vector<size_t> permutation(pSample->size());
    std::iota (std::begin(permutation), std::end(permutation), 0);
    std::shuffle (permutation.begin(), permutation.end(), generator);
    for (size_t j = 0; j < pSample->size(); j++)
        for (size_t k = 0; k < n_folds; k++)
            if (k == j % n_folds)
                v_pSample[k]->push((*pSample)[permutation[j]]);
}

#endif
