#ifndef LIB_SAMPLE_CREATOR_H_
#define LIB_SAMPLE_CREATOR_H_

#include <random>
#include "feature_names.h"
#include "border.h"
#include "sample.h"


class SampleCreator {
public:
    SampleCreator();

    void set_feature_names(std::shared_ptr<FeatureNames>);

    std::shared_ptr<Sample> from_stream(std::istream &);
    std::shared_ptr<Sample> from_file(const std::string &);

    static std::shared_ptr<Sample> merge(const std::shared_ptr<DataContainer> &, const std::shared_ptr<DataContainer> &);

    static std::shared_ptr<Border> lower_border(const std::shared_ptr<Border>&, const std::shared_ptr<Border>&);
    static std::shared_ptr<Border> upper_border(const std::shared_ptr<Border>&, const std::shared_ptr<Border>&);

    /// use graph if available
    template <typename GraphType>
    static std::shared_ptr<Border> lower_border(const std::shared_ptr<Sample>&, const std::shared_ptr<GraphType>&);
    template <typename GraphType>
    static std::shared_ptr<Border> upper_border(const std::shared_ptr<Sample>&, const std::shared_ptr<GraphType>&);

    static std::pair< std::shared_ptr<Sample>, std::shared_ptr<Sample> >
    split_sample(const std::shared_ptr<Sample>&, const boost::dynamic_bitset<> &);

    /// split the sample to train and eval counterparts based on given eval percentage and seed
    static std::pair< std::shared_ptr<Sample>, std::shared_ptr<Sample> >
    split2train_eval(const std::shared_ptr<Sample>&, double, unsigned long);
private:
    std::shared_ptr<FeatureNames> pFN_;

    static boost::dynamic_bitset<> generate_random_bitset(unsigned int, double, unsigned long);
};

SampleCreator::SampleCreator() : pFN_(nullptr){
}

void SampleCreator::set_feature_names(std::shared_ptr<FeatureNames> pFN) {
    pFN_ = std::move(pFN);
}

std::shared_ptr<Sample> SampleCreator::from_stream(std::istream & is) {
    if (!pFN_) throw std::domain_error("You have to set feature_names before importing from stream!");
    std::shared_ptr<Sample> pSample = std::make_shared<Sample>(pFN_->get_feature_indices().size());
    std::string line;
    while (std::getline(is, line)) {
        line = std::regex_replace (line,std::regex("\r$"),"");
        const std::shared_ptr<FeatureVector> pFV =
                std::make_shared<FeatureVector>(line,
                                                pFN_->get_feature_indices(),
                                                pFN_->get_target_feature_index(),
                                                pFN_->get_negatives_label(),
                                                pFN_->get_positives_label(),
                                                pFN_->get_weight_index());
        pSample->push(pFV);
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

std::shared_ptr<Sample> SampleCreator::merge(const std::shared_ptr<DataContainer> & pDC1,
                                             const std::shared_ptr<DataContainer> & pDC2) {
    if (pDC1->dim() != pDC2->dim())
        throw std::domain_error("Unexpected error: trying to merge samples of different dimensions!");
    std::shared_ptr<Sample> pSample = std::make_shared<Sample>(pDC1->dim());
    for (unsigned int i = 0; i < pDC1->size(); ++i)
        pSample->push((*pDC1)[i]);
    for (unsigned int j = 0; j < pDC2->size(); ++j)
        pSample->push((*pDC2)[j]);
    return pSample;
}

std::shared_ptr<Border> SampleCreator::
lower_border(const std::shared_ptr<Border> & pAboveBorder, const std::shared_ptr<Border> & pBelowBorder) {
    if (pAboveBorder->dim() != pBelowBorder->dim())
        throw std::runtime_error("Dimensions of the compared borders do not coincide");
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pBelowBorder->dim());
    for (unsigned int i=0; i < pBelowBorder->size(); i++) {
        pBorder->push((*pBelowBorder)[i]);
    }
    for (unsigned int i=0; i < pAboveBorder->size(); i++) {
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
    for (unsigned int i=0; i < pAboveBorder->size(); i++) {
        pBorder->push((*pAboveBorder)[i]);
    }
    for (unsigned int i=0; i < pBelowBorder->size(); i++) {
        if ( !pAboveBorder->point_below((*pBelowBorder)[i]->get_data()) )
            pBorder->push((*pBelowBorder)[i]);
    }
    return pBorder;
}

template <typename GraphType>
std::shared_ptr<Border> SampleCreator::
lower_border(const std::shared_ptr<Sample> & pSample, const std::shared_ptr<GraphType> & pGraph) {
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pSample->dim());
    const unsigned int size = pSample->size();
    const boost::dynamic_bitset<> & bits = pSample->get_lower_border(pGraph);
    for (unsigned int i = 0; i < size; ++i) {
        if (bits[i]) pBorder->push((*pSample)[i]);
    }
    pBorder->set_neg_pos_counts(pSample->get_neg_pos_counts());
    return pBorder;
}

template <typename GraphType>
std::shared_ptr<Border> SampleCreator::
upper_border(const std::shared_ptr<Sample> & pSample, const std::shared_ptr<GraphType> & pGraph) {
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pSample->dim());
    const unsigned int size = pSample->size();
    const boost::dynamic_bitset<> & bits = pSample->get_upper_border(pGraph);
    for (unsigned int i = 0; i < size; ++i) {
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

    for (unsigned int i = 0; i < pSample->size(); ++i) {
        if (db[i]) pSampleUpper->push((*pSample)[i]);
        else pSampleLower->push((*pSample)[i]);
    }
    return std::pair<std::shared_ptr<Sample>, std::shared_ptr<Sample>>(pSampleLower, pSampleUpper);
}

boost::dynamic_bitset<> SampleCreator::
generate_random_bitset(const unsigned int size, const double probability, const unsigned long seed) {
    boost::dynamic_bitset<> bs(size);

    //std::random_device rd;
    std::mt19937 gen(seed);
    std::bernoulli_distribution d(probability);
    for( unsigned int n = 0; n < size; ++n) {
        bs[ n] = d( gen);
    }
    return bs;
}

std::pair<std::shared_ptr<Sample>, std::shared_ptr<Sample> > SampleCreator::
split2train_eval(const std::shared_ptr<Sample> & pSample, const double eval_percentage, const unsigned long seed) {
    return split_sample(pSample, generate_random_bitset(pSample->size(),eval_percentage,seed));
}

#endif
