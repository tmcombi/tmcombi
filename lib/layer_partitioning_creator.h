#ifndef LIB_LAYER_PARTITIONING_CREATOR_H_
#define LIB_LAYER_PARTITIONING_CREATOR_H_

#include "layer_partitioning.h"

class LayerPartitioningCreator {
public:
    LayerPartitioningCreator();
    LayerPartitioningCreator push_back(const std::shared_ptr<Sample> &);
private:
    std::shared_ptr<LayerPartitioning> layer_partitioning_;
};

LayerPartitioningCreator::LayerPartitioningCreator() : layer_partitioning_(std::make_shared<LayerPartitioning>()){
    if (layer_partitioning_.use_count()!=1)
        throw std::runtime_error("Unexpected error");
}

LayerPartitioningCreator LayerPartitioningCreator::push_back(const std::shared_ptr<Sample> & pSample) {
    layer_partitioning_->push_back(pSample);
    return *this;
}


#endif
