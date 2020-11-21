#define BOOST_TEST_MODULE test_fs_adult_transformed_dataset
#include <boost/test/included/unit_test.hpp>

//#define TIMERS
//#define TRACE_EVALUATOR

#include "../lib/forward_selection.h"



BOOST_AUTO_TEST_CASE( fs_adult_transformed_dataset  ) {
    const std::string names_file("data/adult_transformed/adult_transformed.names");
    const std::string data_file("data/adult_transformed/adult_transformed.data");
    const std::string eval_file("data/adult_transformed/adult_transformed.test");
    BOOST_TEST_MESSAGE("Creating train sample from file: " << data_file);
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << eval_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    std::shared_ptr<Sample> pSampleEval = sample_creator.from_file(eval_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 13);
    BOOST_CHECK_EQUAL(pSample->size(), 26314);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 13906);
    std::shared_ptr<ForwardSelection> pFS = std::make_shared<ForwardSelection>();
    pFS->set_sample(pSample);
    pFS->init();
    /*
    BOOST_CHECK_EQUAL(pFS->try_inactive_features(), true);
    BOOST_CHECK_EQUAL(pFS->try_inactive_features(), true);
    BOOST_CHECK_EQUAL(pFS->try_inactive_features(), true);
    BOOST_CHECK_EQUAL(pFS->try_inactive_features(), true);
    BOOST_CHECK_EQUAL(pFS->try_inactive_features(), true);
     */
    pFS->optimize();
}
