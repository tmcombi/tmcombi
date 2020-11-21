#define BOOST_TEST_MODULE test_fs_tmc_paper_dataset
#include <boost/test/included/unit_test.hpp>

//#define TIMERS
//#define TRACE_EVALUATOR

#include "../lib/forward_selection.h"



BOOST_AUTO_TEST_CASE( tmc_fs_tmc_paper_dataset  ) {
    const std::string names_file("data/tmc_paper/tmc_paper.names");
    const std::string data_file("data/tmc_paper/tmc_paper.data");
    const std::string eval_file("data/tmc_paper/tmc_paper.test");
    BOOST_TEST_MESSAGE("Creating train sample from file: " << data_file);
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << eval_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    std::shared_ptr<Sample> pSampleEval = sample_creator.from_file(eval_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 992);
    std::shared_ptr<ForwardSelection> pFS = std::make_shared<ForwardSelection>();
    pFS->set_sample(pSample);
    pFS->init();
    //BOOST_CHECK_EQUAL(pFS->try_inactive_features(), true);
    //BOOST_CHECK_EQUAL(pFS->try_inactive_features(), false);
    pFS->optimize();
}
