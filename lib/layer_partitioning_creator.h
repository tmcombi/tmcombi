#ifndef LIB_LAYER_PARTITIONING_CREATOR_H_
#define LIB_LAYER_PARTITIONING_CREATOR_H_

//#define DO_CONSISTENCY_CHECKS

#include "layer_partitioning.h"
#include "layer_partitioner.h"

class LayerPartitioningCreator {
public:
    LayerPartitioningCreator();
    LayerPartitioningCreator & push_back(const std::shared_ptr<Sample>&);
    [[nodiscard]] std::shared_ptr<LayerPartitioning> get_layer_partitioning() const;
    bool try_split();
    bool try_merge();
    bool do_one_step();
    size_t optimize();
private:
    std::shared_ptr<LayerPartitioning> layer_partitioning_;
    std::deque<std::shared_ptr<Layer>>::iterator try_split_iterator_;
    std::set<std::shared_ptr<Layer>> split_not_possible_set_;

    bool try_split(std::deque<std::shared_ptr<Layer>>::iterator &);
    bool try_merge(std::deque<std::shared_ptr<Layer>>::iterator &);
};

LayerPartitioningCreator::LayerPartitioningCreator() : layer_partitioning_(std::make_shared<LayerPartitioning>()){
    if (layer_partitioning_.use_count()!=1)
        throw std::runtime_error("Unexpected error");
    try_split_iterator_ = layer_partitioning_->begin();
}

LayerPartitioningCreator & LayerPartitioningCreator::push_back(const std::shared_ptr<Sample>& pSample) {
    layer_partitioning_->push_back(pSample);
    try_split_iterator_ = layer_partitioning_->begin();
    return *this;
}

std::shared_ptr<LayerPartitioning> LayerPartitioningCreator::get_layer_partitioning() const {
    return layer_partitioning_;
}

bool LayerPartitioningCreator::try_split() {
    const auto it = try_split_iterator_;
    for (;try_split_iterator_!=layer_partitioning_->end(); ++try_split_iterator_) {
        if (try_split(try_split_iterator_)) return true;
    }
    for (try_split_iterator_ = layer_partitioning_->begin(); try_split_iterator_!=it; ++try_split_iterator_) {
        if (try_split(try_split_iterator_)) return true;
    }
    if (layer_partitioning_->size() != split_not_possible_set_.size())
        throw std::runtime_error("If we cannot split anything else, "
                                 "then sizes of split_not_possible_set_ and layer_partitioning_ must be equal. "
                                 "They are not!");
    return false;
}

bool LayerPartitioningCreator::try_merge() {
    for (auto it = layer_partitioning_->begin(); it+1 != layer_partitioning_->end(); ++it) {
        if (try_merge(it)) return true;
    }
    return false;
}

bool LayerPartitioningCreator::do_one_step() {
    if (try_split()) return true;
    return try_merge();
}

size_t LayerPartitioningCreator::optimize() {
    size_t counter = 0;
    while (do_one_step()) {

#ifdef DO_CONSISTENCY_CHECKS
        if (!layer_partitioning_->consistent())
            throw std::runtime_error("Got inconsistent layer partitioning");
        if (!layer_partitioning_->consistent())
            throw std::runtime_error("Got inconsistent layer partitioning");
#endif
        counter++;
    }
    return counter;
}

bool LayerPartitioningCreator::try_split(std::deque<std::shared_ptr<Layer>>::iterator & it) {
    if (split_not_possible_set_.find(*it)!=split_not_possible_set_.end()) return false;
    auto pLayerPartitioner = std::make_shared<LayerPartitioner<LayerPartitioning::GraphType> >();
    pLayerPartitioner->set_layer(*it);
    pLayerPartitioner->set_graph(layer_partitioning_->get_graph(*it));
    boost::dynamic_bitset<> mask;
    bool decomposable;
    std::tie(mask, decomposable) = pLayerPartitioner->compute();
    if (decomposable) {
#ifdef TIMERS
        const std::clock_t time1 = std::clock();
#endif
        layer_partitioning_->split_layer(it, mask);
#ifdef TIMERS
        const std::clock_t time2 = std::clock();
        std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Splitting the layer" << std::endl;
#endif
    } else {
        split_not_possible_set_.insert(*it);
    }
    return decomposable;
}

bool LayerPartitioningCreator::try_merge(std::deque<std::shared_ptr<Layer>>::iterator & it) {
    Layer::WeightType n1=0, p1=0, n2=0, p2=0;
    std::tie(n1,p1)=(*it)->get_neg_pos_counts();
    std::tie(n2,p2)=(*(it+1))->get_neg_pos_counts();
    if (p1*n2>=p2*n1) {
        if (split_not_possible_set_.find(*it)!=split_not_possible_set_.end())
            split_not_possible_set_.erase(*it);
        if (split_not_possible_set_.find(*(it+1))!=split_not_possible_set_.end())
            split_not_possible_set_.erase(*(it+1));
        layer_partitioning_->merge_two_layers(it);
        try_split_iterator_ = layer_partitioning_->begin();
        return true;
    }
    return false;
}

#endif
