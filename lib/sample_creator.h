#ifndef LIB_SAMPLE_CREATOR_H_
#define LIB_SAMPLE_CREATOR_H_

#include "feature_names.h"
#include "border.h"
#include "sample.h"

class SampleCreator {
public:
    SampleCreator();

    void set_feature_names(std::shared_ptr<FeatureNames>);

    std::shared_ptr<Sample> from_stream(std::istream &);
    std::shared_ptr<Sample> from_file(const std::string &);
    //do we really need this function?
    static std::shared_ptr<Sample> from_sample(const std::shared_ptr<Sample>&, const std::vector<unsigned int> &);
    static std::shared_ptr<Sample> merge(const std::shared_ptr<DataContainer> &, const std::shared_ptr<DataContainer> &);
    static std::shared_ptr<Border> lower_border(const std::shared_ptr<Sample>&);
    static std::shared_ptr<Border> upper_border(const std::shared_ptr<Sample>&);
    static std::pair< std::shared_ptr<Sample>, std::shared_ptr<Sample> >
    split_sample(const std::shared_ptr<Sample>&, const boost::dynamic_bitset<> &);

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

std::shared_ptr<Sample> SampleCreator::from_sample(const std::shared_ptr<Sample>& pBaseSample,
                                                   const std::vector<unsigned int> & indices) {
    std::shared_ptr<Sample> pSample = std::make_shared<Sample>(pBaseSample->dim());
    for (unsigned int index : indices)
        pSample->push((*pBaseSample)[index]);
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

std::shared_ptr<Border> SampleCreator::lower_border(const std::shared_ptr<Sample> & pSample) {
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pSample->dim());
    const unsigned int size = pSample->size();
    const boost::dynamic_bitset<> & bits = pSample->get_lower_border();
    for (unsigned int i = 0; i < size; ++i) {
        if (bits[i]) pBorder->push((*pSample)[i]);
    }
    pBorder->set_neg_pos_counts(pSample->get_neg_pos_counts());
    return pBorder;
}

std::shared_ptr<Border> SampleCreator::upper_border(const std::shared_ptr<Sample> & pSample) {
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pSample->dim());
    const unsigned int size = pSample->size();
    const boost::dynamic_bitset<> & bits = pSample->get_upper_border();
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

#endif
