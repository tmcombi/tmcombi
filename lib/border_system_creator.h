#ifndef LIB_BORDER_SYSTEM_CREATOR_H_
#define LIB_BORDER_SYSTEM_CREATOR_H_

//#define TRACE_BORDER_SYSTEM

#include "border_system.h"
#include "layer_partitioning.h"
#include "sample_creator.h"
#ifdef TIMERS
#include "timers.h"
#endif

class BorderSystemCreator {
public:
    static std::shared_ptr<BorderSystem> from_layer_partitioning(const std::shared_ptr<LayerPartitioning>&) ;
};

std::shared_ptr<BorderSystem> BorderSystemCreator::
from_layer_partitioning(const std::shared_ptr<LayerPartitioning> & pLP) {
    const size_t dim = pLP->dim();
    const size_t size = pLP->size();
    std::shared_ptr<BorderSystem> pBS = std::make_shared<BorderSystem>(dim, size);
#ifdef TIMERS
    auto h = Timers::server().start("creation of upper borders");
#endif
    size_t counter = 0;
    Sample::WeightType cumulative_neg = 0, cumulative_pos = 0;
    for (auto it = pLP->begin(); it != pLP->end(); ++it) {
        const auto & pGraph = pLP->get_graph(*it);
        const std::shared_ptr<Border> pLayerUpperPart = SampleCreator::upper_border(*it, pGraph);
        const auto & neg_pos_counts = (*it)->get_neg_pos_counts();
        cumulative_neg += neg_pos_counts.first;
        cumulative_pos += neg_pos_counts.second;
        pBS->cumulative_neg_pos_[counter] = {cumulative_neg, cumulative_pos};
        pBS->pUpperBorder_[counter++] = pLayerUpperPart;
    }
#ifdef TIMERS
    std::cout << Timers::server().stop(h) << std::endl;
    h = Timers::server().start("creation of lower borders");
#endif
    for (auto it = pLP->rbegin(); it != pLP->rend(); ++it) {
        const auto & pGraph = pLP->get_graph(*it);
        const std::shared_ptr<Border> pLayerLowerPart = SampleCreator::lower_border(*it, pGraph);
        pBS->pLowerBorder_[--counter] = pLayerLowerPart;
    }
#ifdef TIMERS
    std::cout << Timers::server().stop(h) << std::endl;
#endif

#ifdef TRACE_BORDER_SYSTEM
    std::cout << "## TRACE_BORDER_SYSTEM: size=" << pBS->size() << std::endl;
    double neg=0, pos=0;
    for (const auto & it : *pLP) {
        std::tie(neg,pos) = (*it).get_neg_pos_counts();
        std::cout << "## TRACE_BORDER_SYSTEM: (neg, pos) = (" << neg << ", " << pos
        << "), conf = " << pos/(neg+pos) << std::endl;
    }
#endif
    return pBS;
}

#endif
