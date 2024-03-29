#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_classifier_creator_fs_density
#endif
#include <boost/test/included/unit_test.hpp>

#include "feature_names.h"
#include "classifier_creator_train_tmc.h"
#include "classifier_creator_fs_density.h"

BOOST_AUTO_TEST_SUITE( classifier_creator_fs_density )

    BOOST_AUTO_TEST_CASE( basic )
    {
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

        std::shared_ptr<ClassifierCreatorTrain> pTC_aux = std::make_shared<ClassifierCreatorTrainTmc>();
        std::shared_ptr<ClassifierCreatorFsDensity> pCCFS = std::make_shared<ClassifierCreatorFsDensity>();
        pCCFS->verbose(true);
        (*pCCFS).set_classifier_creator_train(pTC_aux).init(pSample);
        (*pCCFS).train();

        std::shared_ptr<Classifier> pFsClTmc = pCCFS->get_classifier();

        std::vector<double> p;  std::pair<double, double> conf;
        p = {10,2}; conf = {0.625,0.625}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), conf.first);
        p = {16,1}; conf = {0.625,0.625}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), conf.first);
        p = {7,10}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.49645390070921985);
        p = {11,10}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.49645390070921985);
        p = {4,6}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), conf.second);
        p = {7,13}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.49645390070921985);
        p = {14.5,0.5}; conf = {0.625,1}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.6470588235294118);
        p = {-1,16}; conf = {0,0}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0);
        p = {-1,18}; conf = {0,0}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0);
        p = {12.5,1.5}; conf = {0.625,0.625}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.625);
        p = {4,8}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.49645390070921985);
        p = {3,13}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.49645390070921985);
        p = {1,18};  conf = {0,0}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0);
        p = {15,5.5}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.49645390070921985);
        p = {18,1}; conf = {0.625,0.625}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.625);
        p = {8,12}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), 0.49645390070921985);
     }

    BOOST_AUTO_TEST_CASE( set_starting_fm )
    {
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

        std::shared_ptr<ClassifierCreatorTrain> pTC_aux = std::make_shared<ClassifierCreatorTrainTmc>();
        std::shared_ptr<ClassifierCreatorFsDensity> pCCFS = std::make_shared<ClassifierCreatorFsDensity>();
        pCCFS->verbose(true);
        (*pCCFS).set_classifier_creator_train(pTC_aux).init(pSample);
        (*pCCFS).set_starting_feature_mask(std::make_shared<FeatureMask>("01","01"));
        (*pCCFS).train();

        std::shared_ptr<Classifier> pFsClTmc = pCCFS->get_classifier();

        std::vector<double> p;  std::pair<double, double> conf;
        p = {16,1}; conf = {0.53846153846153844,0.53846153846153844}; BOOST_CHECK(pFsClTmc->confidence_interval(p) == conf);
        BOOST_CHECK_EQUAL(pFsClTmc->confidence(p), conf.first);
    }


BOOST_AUTO_TEST_SUITE_END()
