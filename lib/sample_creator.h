#ifndef LIB_SAMPLE_CREATOR_H_
#define LIB_SAMPLE_CREATOR_H_

#include "feature_names.h"
#include "border.h"
#include "sample.h"

class SampleCreator {
public:
    explicit SampleCreator();

    void set_feature_names(std::shared_ptr<FeatureNames>);

    std::shared_ptr<Sample> from_stream(std::istream &);
    std::shared_ptr<Sample> from_file(const std::string &);
    //do we really need this function?
    static std::shared_ptr<Sample> from_sample(const std::shared_ptr<Sample>&, const std::vector<unsigned int> &);
    static std::shared_ptr<Sample> merge(const std::shared_ptr<Sample> &, const std::shared_ptr<Sample> &);
    static std::shared_ptr<Sample> lower_border(const std::shared_ptr<Sample>&);
    static std::shared_ptr<Sample> upper_border(const std::shared_ptr<Sample>&);
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
    std::ifstream fs_data(data_file);
    if (!fs_data.is_open())
        throw std::runtime_error("Cannot open file: " + data_file);
    std::shared_ptr<Sample> pSample = from_stream(fs_data);
    fs_data.close();
    return pSample;
}

std::shared_ptr<Sample> SampleCreator::from_sample(const std::shared_ptr<Sample>& pBaseSample,
                                                   const std::vector<unsigned int> & indices) {
    std::shared_ptr<Sample> pSample = std::make_shared<Sample>(pBaseSample->get_dim());
    for (unsigned int index : indices)
        pSample->push((*pBaseSample)[index]);
    return pSample;
}

std::shared_ptr<Sample> SampleCreator::merge(const std::shared_ptr<Sample> & pSample1,
                                             const std::shared_ptr<Sample> & pSample2) {
    if (pSample1->get_dim() != pSample2->get_dim())
        throw std::domain_error("Unexpected error: trying to merge samples of different dimensions!");
    std::shared_ptr<Sample> pSample = std::make_shared<Sample>(pSample1->get_dim());
    for (unsigned int i = 0; i < pSample1->get_size(); ++i)
        pSample->push((*pSample1)[i]);
    for (unsigned int j = 0; j < pSample2->get_size(); ++j)
        pSample->push((*pSample2)[j]);
    return pSample;
}

std::shared_ptr<Sample> SampleCreator::lower_border(const std::shared_ptr<Sample> & pSample) {
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pSample->get_dim());
    const unsigned int size = pSample->get_size();
    const boost::dynamic_bitset<> & bits = pSample->get_lower_border();
    for (unsigned int i = 0; i < size; ++i) {
        if (bits[i]) pBorder->push((*pSample)[i]);
    }
    pBorder->set_neg_pos_counts(pSample->get_neg_pos_counts());
    return pBorder;
}

std::shared_ptr<Sample> SampleCreator::upper_border(const std::shared_ptr<Sample> & pSample) {
    std::shared_ptr<Border> pBorder = std::make_shared<Border>(pSample->get_dim());
    const unsigned int size = pSample->get_size();
    const boost::dynamic_bitset<> & bits = pSample->get_upper_border();
    for (unsigned int i = 0; i < size; ++i) {
        if (bits[i]) pBorder->push((*pSample)[i]);
    }
    pBorder->set_neg_pos_counts(pSample->get_neg_pos_counts());
    return pBorder;
}


std::pair<std::shared_ptr<Sample>, std::shared_ptr<Sample> >
        SampleCreator::split_sample(const std::shared_ptr<Sample> & pSample, const boost::dynamic_bitset<> & db) {
    if (pSample->get_size() != db.size())
        throw std::domain_error("Sample and bitset have to be of the same size!");

    std::shared_ptr<Sample> pSampleLower = std::make_shared<Sample>(pSample->get_dim());
    std::shared_ptr<Sample> pSampleUpper = std::make_shared<Sample>(pSample->get_dim());

    for (unsigned int i = 0; i < pSample->get_size(); ++i) {
        if (db[i]) pSampleUpper->push((*pSample)[i]);
        else pSampleLower->push((*pSample)[i]);
    }
    return std::pair<std::shared_ptr<Sample>, std::shared_ptr<Sample>>(pSampleLower, pSampleUpper);
}

#endif
