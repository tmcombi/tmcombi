#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_train_classifier_transformed_features
#endif
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "classifier_transformed_features.h"
#include "feature_names.h"
#include "classifier_creator_train_tmc.h"
#include "feature_transform_subset.h"

BOOST_AUTO_TEST_SUITE( classifier_transformed_features )

BOOST_AUTO_TEST_CASE( basics )
{
    const std::string names_file("data/4layers_36points/4layers_36points.names");
    const std::string data_file("data/4layers_36points/4layers_36points.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Original sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 36);

    auto pFT = std::make_shared<FeatureTransformSubset>(std::make_shared<FeatureMask>("10"));
    std::shared_ptr<Sample> pSampleTransformed = std::make_shared<Sample>(1);
    for ( size_t i = 0; i < pSample->size(); i++) {
        const auto pFV = pFT->transform_feature_vector((*pSample)[i]);
        pSampleTransformed->push(pFV);
    }
    BOOST_TEST_MESSAGE("Transformed sample: " << *pSampleTransformed);
    BOOST_CHECK_EQUAL(pSampleTransformed->dim(), 1);
    BOOST_CHECK_EQUAL(pSampleTransformed->size(), 17);

    std::shared_ptr<ClassifierCreatorTrain> pTC = std::make_shared<ClassifierCreatorTrainTmc>();
    (*pTC).init(pSampleTransformed).train();

    std::shared_ptr<Classifier> pClTmc = pTC->get_classifier();

    BOOST_CHECK_EQUAL(pClTmc->confidence({0}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTmc->confidence({1}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTmc->confidence({2}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTmc->confidence({3}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTmc->confidence({4}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTmc->confidence({5}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTmc->confidence({6}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTmc->confidence({7}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTmc->confidence({8}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTmc->confidence({9}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTmc->confidence({10}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTmc->confidence({11}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTmc->confidence({12}), 0.63492063492063489);

    std::shared_ptr<Classifier> pClTrF = std::make_shared<ClassifierTransformedFeatures>(pClTmc,pFT);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,0}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,1}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,2}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,3}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,4}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,5}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,6}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,7}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,8}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,9}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,10}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,11}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTrF->confidence({123,12}), 0.63492063492063489);

}

BOOST_AUTO_TEST_SUITE_END()
