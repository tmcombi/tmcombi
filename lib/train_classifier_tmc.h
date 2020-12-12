#ifndef LIB_TRAIN_CLASSIFIER_TMC_H_
#define LIB_TRAIN_CLASSIFIER_TMC_H_

#include "border_system_creator.h"
#include "classifier_tmc.h"
#include "layer_partitioning_creator.h"
#include "train_classifier.h"

#include <utility>

class TrainClassifierTmc : public TrainClassifier {
public:
    explicit TrainClassifierTmc(std::shared_ptr<Sample>);

    void train() override;

    std::shared_ptr<Classifier> get_classifier() const override;
    void dump_configuration_to_ptree(boost::property_tree::ptree &) const override;
private:
    std::shared_ptr<BorderSystem> pBS_;
    bool trained_;
};

void TrainClassifierTmc::train() {
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample_);
    pLayerPartitioningCreator->optimize();
    std::shared_ptr<LayerPartitioning> pLD(pLayerPartitioningCreator->get_layer_partitioning());

    std::shared_ptr<BorderSystemCreator> pBSC = std::make_shared<BorderSystemCreator>();
    pBS_ = pBSC->from_layer_partitioning(pLD);
    trained_ = true;
}

std::shared_ptr<Classifier> TrainClassifierTmc::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTmc>(pBS_);
}

void TrainClassifierTmc::dump_configuration_to_ptree(boost::property_tree::ptree & pt) const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling dump_configuration_to_ptree()");
    pBS_->dump_to_ptree(pt);
}

TrainClassifierTmc::TrainClassifierTmc(std::shared_ptr<Sample> pSample) :
TrainClassifier(std::move(pSample)), pBS_(nullptr), trained_(false) {
}


#endif