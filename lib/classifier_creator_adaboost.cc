#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_classifier_creator_adaboost
#endif
#include <boost/test/included/unit_test.hpp>

#include "feature_names.h"
#include "classifier_creator_train_tmc.h"
#include "classifier_creator_adaboost.h"

//BOOST_AUTO_TEST_SUITE( classifier_creator_adaboost )

BOOST_AUTO_TEST_CASE( classifier_creator_tmc )
{
    std::string names_file, data_file;

    names_file = "data/4layers_36points/4layers_36points.names";
    data_file = "data/4layers_36points/4layers_36points.data";
    BOOST_TEST_MESSAGE("Creating training sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 36);

    names_file = "data/tmc_paper_9/tmc_paper.names";
    data_file  = "data/tmc_paper_9/tmc_paper.data";
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN9 = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator9;
    sample_creator9.set_feature_names(pFN9);
    std::shared_ptr<Sample> pSample9 = sample_creator9.from_file(data_file);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample9);
    BOOST_CHECK_EQUAL(pSample9->dim(), 2);
    BOOST_CHECK_EQUAL(pSample9->size(), 4);

    std::shared_ptr<ClassifierCreatorTrain> pCC_aux = std::make_shared<ClassifierCreatorTrainTmc>();
    std::shared_ptr<ClassifierCreatorAdaboost> pCCAdaBoost = std::make_shared<ClassifierCreatorAdaboost>();
    pCCAdaBoost->verbose(true);
    (*pCCAdaBoost).set_classifier_creator_train(pCC_aux).
            init(pSample).
            set_eval_sample(pSample9).
            train();

    std::shared_ptr<Classifier> pFsClBoosted = pCCAdaBoost->get_classifier();

    std::vector<double> p;  std::pair<double, double> conf;
    p = {10,2}; conf = {0.625,0.625}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), conf.first);
    p = {16,1}; conf = {0.625,0.625}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), conf.first);
    p = {7,10}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.49645390070921985);
    p = {11,10}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.49645390070921985);
    p = {4,6}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), conf.second);
    p = {7,13}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.49645390070921985);
    p = {14.5,0.5}; conf = {0.625,1}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.6470588235294118);
    p = {-1,16}; conf = {0,0}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0);
    p = {-1,18}; conf = {0,0}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0);
    p = {12.5,1.5}; conf = {0.625,0.625}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.625);
    p = {4,8}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.49645390070921985);
    p = {3,13}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.49645390070921985);
    p = {1,18};  conf = {0,0}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0);
    p = {15,5.5}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.49645390070921985);
    p = {18,1}; conf = {0.625,0.625}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.625);
    p = {8,12}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClBoosted->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pFsClBoosted->confidence(p), 0.49645390070921985);
}

//BOOST_AUTO_TEST_SUITE_END()
