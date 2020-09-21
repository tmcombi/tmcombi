#ifndef LIB_SAMPLE_CREATOR_H_
#define LIB_SAMPLE_CREATOR_H_

#include <utility>

#include "feature_names.h"
#include "sample.h"

class SampleCreator {
public:
    explicit SampleCreator();

    void set_feature_names(std::shared_ptr<FeatureNames>);

    std::shared_ptr<Sample> from_stream(std::istream &);
    std::shared_ptr<Sample> from_file(const std::string &);
    static std::shared_ptr<Sample> from_sample(const std::shared_ptr<Sample>&, const std::vector<unsigned int> &);
    //todo
    std::shared_ptr<Sample> merge(const std::shared_ptr<Sample> &, const std::shared_ptr<Sample> &);
    //todo
    std::shared_ptr<Sample> lower_border(const std::shared_ptr<Sample> &);
    //todo
    std::shared_ptr<Sample> upper_border(const std::shared_ptr<Sample> &);
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
    for (unsigned int i = 0; i < indices.size(); ++i)
        pSample->push((*pBaseSample)[indices[i]]);
    return pSample;
}

std::shared_ptr<Sample> SampleCreator::merge(const std::shared_ptr<Sample> &, const std::shared_ptr<Sample> &) {
    return std::shared_ptr<Sample>();
}

std::shared_ptr<Sample> SampleCreator::lower_border(const std::shared_ptr<Sample> &) {
    return std::shared_ptr<Sample>();
}

std::shared_ptr<Sample> SampleCreator::upper_border(const std::shared_ptr<Sample> &) {
    return std::shared_ptr<Sample>();
}


#endif
