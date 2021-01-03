#define BOOST_TEST_MODULE lib_classifier_creator_feature_selection
#include <boost/test/included/unit_test.hpp>

#include "feature_names.h"
#include "classifier_creator_train_tmc.h"
#include "classifier_creator_feature_selection.h"

BOOST_AUTO_TEST_CASE( classifier_creator_feature_selection_basics_weights )
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
    std::shared_ptr<ClassifierCreatorFeatureSelection> pCCFS = std::make_shared<ClassifierCreatorFeatureSelection>();
    pCCFS->verbose(true);
    (*pCCFS).set_classifier_creator_train(pTC_aux).
    init(pSample).
    set_n_folds(5).
    set_folding_type(ClassifierCreatorFeatureSelection::weights).
    train();

    std::shared_ptr<Classifier> pClTmc = pCCFS->get_classifier();

    std::vector<double> p;  std::pair<double, double> conf;
    p = {10,2}; conf = {1.0,1.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {16,1}; conf = {0.53846153846153844,0.53846153846153844}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {7,10}; conf = {0.42857142857142855,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.70833333333333337);
    p = {11,10}; conf = {0,0.21621621621621623}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.14545454545454545);
    p = {4,6}; conf = {1,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.second);
    p = {7,13}; conf = {0,0.42857142857142855}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.2289156626506024);
    p = {14.5,0.5}; conf = {0.53846153846153844,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.75609756097560976);
    p = {-1,16}; conf = {0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {-1,18}; conf = {0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {12.5,1.5}; conf = {1,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 1);
    p = {4,8}; conf = {1,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 1);
    p = {3,13}; conf = {0.59259259259259256,0.59259259259259256}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.59259259259259256);
    p = {1,18};  conf = {0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {15,5.5}; conf = {0.35714285714285715,0.53846153846153844}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.43902439024390244);
    p = {18,1}; conf = {0,0.53846153846153844}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.27083333333333331);
    p = {8,12}; conf = {0.42857142857142855,0.42857142857142855}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.42857142857142855);
}

BOOST_AUTO_TEST_CASE( classifier_creator_feature_selection_basics_split )
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
    std::shared_ptr<ClassifierCreatorFeatureSelection> pCCFS = std::make_shared<ClassifierCreatorFeatureSelection>();
    pCCFS->verbose(true);
    (*pCCFS).set_classifier_creator_train(pTC_aux).
            init(pSample).
            set_n_folds(5).
            set_folding_type(ClassifierCreatorFeatureSelection::split).
            train();

    std::shared_ptr<Classifier> pClTmc = pCCFS->get_classifier();

    std::vector<double> p;  std::pair<double, double> conf;
    p = {10,2}; conf = {0.625,0.625}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {16,1}; conf = {0.625,0.625}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {7,10}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.49645390070921985);
    p = {11,10}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.49645390070921985);
    p = {4,6}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.second);
    p = {7,13}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.49645390070921985);
    p = {14.5,0.5}; conf = {0.625,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.6470588235294118);
    p = {-1,16}; conf = {0,0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0);
    p = {-1,18}; conf = {0,0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0);
    p = {12.5,1.5}; conf = {0.625,0.625}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.625);
    p = {4,8}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.49645390070921985);
    p = {3,13}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.49645390070921985);
    p = {1,18};  conf = {0,0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0);
    p = {15,5.5}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.49645390070921985);
    p = {18,1}; conf = {0.625,0.625}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.625);
    p = {8,12}; conf = {0.49645390070921985,0.49645390070921985}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), 0.49645390070921985);
}
