#define BOOST_TEST_MODULE lib_test_forward_selection
#include <boost/test/included/unit_test.hpp>

#include "forward_selection.h"

BOOST_AUTO_TEST_CASE( forward_selection_36points_example ) {
    const std::string names_file("data/4layers_36points/4layers_36points.names");
    const std::string data_file("data/4layers_36points/4layers_36points.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 36);

    std::shared_ptr<ForwardSelection> pFS = std::make_shared<ForwardSelection>();
    pFS->set_sample(pSample);
    pFS->init();
    BOOST_CHECK_EQUAL(pFS->try_inactive_feature(0,false).first, 0.5313852813852814);
    //BOOST_CHECK_EQUAL(pFS->try_inactive_features(), true);
    //BOOST_CHECK_EQUAL(pFS->try_inactive_features(), false);
    pFS->optimize();
}
