#ifndef LIB_BORDER_SYSTEM_CREATOR_H_
#define LIB_BORDER_SYSTEM_CREATOR_H_

//#define TRACE_BORDER_SYSTEM

#include "border_system.h"
#include "layer_partitioning.h"
#include "sample_creator.h"

class BorderSystemCreator {
public:
    static std::shared_ptr<BorderSystem> from_layer_partitioning(const std::shared_ptr<LayerPartitioning>&) ;
};

std::shared_ptr<BorderSystem> BorderSystemCreator::
from_layer_partitioning(const std::shared_ptr<LayerPartitioning> & pLP) {
    const unsigned int dim = pLP->dim();
    const unsigned int size = pLP->size();
    std::shared_ptr<BorderSystem> pBS = std::make_shared<BorderSystem>(dim, size);
#ifdef TIMERS
    const std::clock_t time1 = std::clock();
#endif
    unsigned int counter = 0;
    std::shared_ptr<Border> pCurrentUpper = std::make_shared<Border>(dim);
    for (auto it = pLP->begin(); it != pLP->end(); ++it) {
        const auto & pGraph = pLP->get_graph(*it);
        const std::shared_ptr<Border> pLayerUpperPart = SampleCreator::upper_border(*it, pGraph);
        pCurrentUpper = SampleCreator::upper_border(pCurrentUpper, pLayerUpperPart);
        pCurrentUpper->set_neg_pos_counts((*it)->get_neg_pos_counts());
        pBS->pUpperBorder_[counter++] = pCurrentUpper;
    }
#ifdef TIMERS
    const std::clock_t time2 = std::clock();
    std::cout << "Timers: " << (time2-time1)/(CLOCKS_PER_SEC/1000) << "ms - Creation of upper borders" << std::endl;
#endif
    std::shared_ptr<Border> pCurrentLower = std::make_shared<Border>(dim);
    for (auto it = pLP->rbegin(); it != pLP->rend(); ++it) {
        const auto & pGraph = pLP->get_graph(*it);
        const std::shared_ptr<Border> pLayerLowerPart = SampleCreator::lower_border(*it, pGraph);
        pCurrentLower = SampleCreator::lower_border(pCurrentLower, pLayerLowerPart);
        pCurrentLower->set_neg_pos_counts((*it)->get_neg_pos_counts());
        pBS->pLowerBorder_[--counter] = pCurrentLower;
    }
#ifdef TIMERS
    const std::clock_t time3 = std::clock();
    std::cout << "Timers: " << (time3-time2)/(CLOCKS_PER_SEC/1000) << "ms - Creation of lower borders" << std::endl;
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
