#ifndef LIB_BORDER_SYSTEM_CREATOR_H_
#define LIB_BORDER_SYSTEM_CREATOR_H_

#include "border_system.h"
#include "layer_partitioning.h"
#include "sample_creator.h"

class BorderSystemCreator {
public:
    static std::shared_ptr<BorderSystem> from_layer_partitioning(const std::shared_ptr<LayerPartitioning>&) ;
};

std::shared_ptr<BorderSystem> BorderSystemCreator::
from_layer_partitioning(const std::shared_ptr<LayerPartitioning> & pLP) {
    const SampleCreator sample_creator;
    const unsigned int dim = pLP->dim();
    const unsigned int size = pLP->size();
    std::shared_ptr<BorderSystem> pBS = std::make_shared<BorderSystem>(pLP->dim(), size);

    unsigned int counter = 0;
    std::shared_ptr<Border> pCurrentUpper = std::make_shared<Border>(dim);
    for (const auto & it : *pLP) {
        const std::shared_ptr<Border> pLayerUpperPart = sample_creator.upper_border(it);
        const std::shared_ptr<Sample> pCurrentUpperMergedWithLayerUpperPart =
                sample_creator.merge(pCurrentUpper,pLayerUpperPart);
        pCurrentUpper = sample_creator.upper_border(pCurrentUpperMergedWithLayerUpperPart);
        pCurrentUpper->set_neg_pos_counts(it->get_neg_pos_counts());
        pBS->pUpperBorder_[counter++] = pCurrentUpper;
    }

    std::shared_ptr<Border> pCurrentLower = std::make_shared<Border>(dim);
    for (auto it = pLP->rbegin(); it != pLP->rend(); ++it) {
        const std::shared_ptr<Border> pLayerLowerPart = sample_creator.lower_border(*it);
        const std::shared_ptr<Sample> pCurrentLowerMergedWithLayerLowerPart =
                sample_creator.merge(pCurrentLower,pLayerLowerPart);
        pCurrentLower = sample_creator.lower_border(pCurrentLowerMergedWithLayerLowerPart);
        pCurrentLower->set_neg_pos_counts((*it)->get_neg_pos_counts());
        pBS->pLowerBorder_[--counter] = pCurrentLower;
    }
    return pBS;
}

#endif
