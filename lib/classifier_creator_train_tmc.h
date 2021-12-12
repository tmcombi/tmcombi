#ifndef LIB_CLASSIFIER_CREATOR_TRAIN_TMC_H_
#define LIB_CLASSIFIER_CREATOR_TRAIN_TMC_H_

#include "border_system_creator.h"
#include "classifier_tmc.h"
#include "layer_partitioning_creator.h"
#include "classifier_creator_train.h"

#include <utility>

class ClassifierCreatorTrainTmc : public ClassifierCreatorTrain {
public:
    ClassifierCreatorTrainTmc();

    ClassifierCreatorTrainTmc & init(const std::shared_ptr<Sample> &) override;

    ClassifierCreatorTrainTmc & train() override;

    [[nodiscard]] std::shared_ptr<Classifier> get_classifier() const override;
private:
    std::shared_ptr<BorderSystem> pBS_;
    bool trained_;
};

ClassifierCreatorTrainTmc::ClassifierCreatorTrainTmc() : pBS_(nullptr), trained_(false) {
}

ClassifierCreatorTrainTmc &ClassifierCreatorTrainTmc::init(const std::shared_ptr<Sample> & pSample) {
    ClassifierCreatorTrain::init(pSample);
    pBS_ = nullptr;
    trained_ = false;
    return *this;
}

ClassifierCreatorTrainTmc &ClassifierCreatorTrainTmc::train() {
    if ( get_sample() == nullptr ) throw std::runtime_error("specify sample prior training");
    if (verbose()) std::cout << "Starting tmc optimization... " << std::flush;
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(get_sample());
    const auto iteration_num = pLayerPartitioningCreator->optimize();
    if (verbose()) std::cout << "finished in " << iteration_num << " iterations" << std::endl;
    std::shared_ptr<LayerPartitioning> pLP(pLayerPartitioningCreator->get_layer_partitioning());
    if (verbose()) {
        const auto layer_partitioning_size = pLP->size();
        std::cout << "Resulting layer partitioning size: " << layer_partitioning_size << std::endl;
    }
    std::shared_ptr<BorderSystemCreator> pBSC = std::make_shared<BorderSystemCreator>();
    pBS_ = pBSC->from_layer_partitioning(pLP);
    trained_ = true;
    return *this;
}

std::shared_ptr<Classifier> ClassifierCreatorTrainTmc::get_classifier() const {
    if (!trained_)
        throw std::runtime_error("Use train() to train before calling get_classifier()");
    return std::make_shared<ClassifierTmc>(pBS_);
}



#endif