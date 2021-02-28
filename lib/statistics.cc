#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_statistics
#endif
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"
#include "statistics.h"

BOOST_AUTO_TEST_SUITE( statistics )

BOOST_AUTO_TEST_CASE( basics ) {
    const std::string names_file("data/4layers_36points/4layers_36points.names");
    const std::string data_file("data/4layers_36points/4layers_36points.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);

    std::shared_ptr<Statistics> pStat = std::make_shared<Statistics>();
    pStat->set_sample(pSample);

    std::vector<double> roc_err_feature_wise = pStat->get_roc_err_feature_wise();
    std::vector<double> roc_err_feature_wise_truth = {0.62037037037037035, 0.42012051734273959};
    BOOST_CHECK_EQUAL(roc_err_feature_wise.size(), pSample->dim());
    BOOST_CHECK(roc_err_feature_wise == roc_err_feature_wise_truth);
}

// deactivated - too slow for unit tests
/*

BOOST_AUTO_TEST_CASE( adult_transformed_train ) {
    const std::string names_file("data/adult_transformed/adult_transformed.names");
    const std::string data_file("data/adult_transformed/adult_transformed.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    std::shared_ptr<Statistics> pStat = std::make_shared<Statistics>();
    pStat->set_sample(pSample);

    std::vector<double> roc_err_feature_wise = pStat->get_roc_err_feature_wise();
    std::vector<double> roc_err_feature_wise_truth =
            {
                    0.30634579092209485, //0
                    0.42769147135374408, //1
                    0.27934285895449901, //2
                    0.27960029864515201, //3
                    0.22902236366873585, //4
                    0.26760587253936674, //5
                    0.2180471190592701, //6
                    0.45975429586509547, //7
                    0.38530751285635184, //8
                    0.40978774816374031, //9
                    0.46495236678005702, //10
                    0.32418338655715939, //11
                    0.46103110261550934 //12
            };
    BOOST_CHECK_EQUAL(roc_err_feature_wise.size(), pSample->dim());
    BOOST_CHECK(roc_err_feature_wise == roc_err_feature_wise_truth);
}

BOOST_AUTO_TEST_CASE( adult_transformed_test ) {
    const std::string names_file("data/adult_transformed/adult_transformed.names");
    const std::string data_file("data/adult_transformed/adult_transformed.test");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    std::shared_ptr<Statistics> pStat = std::make_shared<Statistics>();
    pStat->set_sample(pSample);

    std::vector<double> roc_err_feature_wise = pStat->get_roc_err_feature_wise();
    std::vector<double> roc_err_feature_wise_truth =
            {
                    0.31314848393289102,
                    0.42737295704022649,
                    0.27851560171508266,
                    0.27836509286022781,
                    0.22540565499449142,
                    0.27635992597746717,
                    0.21662670560500127,
                    0.4545244508451039,
                    0.38687598202016815,
                    0.412032568184728,
                    0.46559906717601263,
                    0.32971863307904059,
                    0.46834402299905292
            };
    BOOST_CHECK_EQUAL(roc_err_feature_wise.size(), pSample->dim());
    BOOST_CHECK(roc_err_feature_wise == roc_err_feature_wise_truth);
}
*/

BOOST_AUTO_TEST_SUITE_END()