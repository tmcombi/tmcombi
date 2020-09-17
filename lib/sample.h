#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <utility>

#include "feature_names.h"
#include "feature_vector.h"

class Sample {
public:
    virtual unsigned int get_dim() const = 0;
    virtual unsigned int get_size() const = 0;
    virtual const FeatureVector& operator[](unsigned int) const = 0;
};

class ContainerSample : virtual public Sample {
public:
    explicit ContainerSample(std::shared_ptr<FeatureNames> );

    void push(const std::shared_ptr<FeatureVector>& );
    void push_from_stream(std::istream &);

    unsigned int get_dim() const override;
    unsigned int get_size() const override;
    const FeatureVector& operator[](unsigned int) const override;

private:
    const std::shared_ptr<FeatureNames> pFN_;
    std::vector<std::shared_ptr<FeatureVector>> pFV_;
    std::map<const std::vector<double>,unsigned int> fv2index_map_;
};

ContainerSample::ContainerSample(std::shared_ptr<FeatureNames>  pFN): pFN_(std::move(pFN)) {
}

unsigned int ContainerSample::get_dim() const {
    return pFN_->get_dim();
}

unsigned int ContainerSample::get_size() const {
    return pFV_.size();
}

void ContainerSample::push(const std::shared_ptr<FeatureVector>& pFV) {
    if ( fv2index_map_.find(pFV->get_data()) == fv2index_map_.end() ) {
        fv2index_map_[pFV->get_data()] = pFV_.size();
        pFV_.push_back(pFV);
    }  else {
        const unsigned int offset = fv2index_map_[pFV->get_data()];
        pFV_[offset]->inc_weight_positives(pFV->get_weight_positives());
        pFV_[offset]->inc_weight_negatives(pFV->get_weight_negatives());
    }
}

const FeatureVector &ContainerSample::operator[](unsigned int i) const {
    return *pFV_[i];
}

void ContainerSample::push_from_stream(std::istream & is) {
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
        push(pFV);
    }
}

class SubSample : virtual public Sample {
public:
    explicit SubSample(std::shared_ptr<Sample>, std::vector<unsigned int>);

    unsigned int get_dim() const override;
    unsigned int get_size() const override;
    const FeatureVector& operator[](unsigned int) const override;

private:
    const std::shared_ptr<Sample> pBaseSample_;
    const std::vector<unsigned int> vSubIndex_;
};


SubSample::SubSample(std::shared_ptr<Sample> pBaseSample, std::vector<unsigned int> vSubIndex) :
pBaseSample_(std::move(pBaseSample)), vSubIndex_(std::move(vSubIndex)) {
}

unsigned int SubSample::get_dim() const {
    return pBaseSample_->get_dim();
}

unsigned int SubSample::get_size() const {
    return vSubIndex_.size();
}

const FeatureVector &SubSample::operator[](unsigned int i) const {
    return pBaseSample_->operator[](vSubIndex_[i]);
}

std::ostream &operator<<(std::ostream & stream, const Sample & sample) {
    if (sample.get_size()) {
        stream << sample[0];
    }
    for (unsigned int i = 1; i < sample.get_size(); ++i) {
        stream << ';' << sample[i];
    }
    return stream;
}

#endif
