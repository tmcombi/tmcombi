#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_classifier_creator_dispatch_ptree
#endif
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "classifier_creator_train_tmc.h"
#include "classifier_creator_dispatch_ptree.h"
#include "feature_names.h"
#include "feature_transform_subset.h"

BOOST_AUTO_TEST_SUITE( classifier_creator_dispatch_ptree )

BOOST_AUTO_TEST_CASE( tmc )
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

    std::shared_ptr<ClassifierCreatorTrain> pTC = std::make_shared<ClassifierCreatorTrainTmc>();
    (*pTC).init(pSample).train();
    boost::property_tree::ptree pt;
    pTC->get_classifier()->dump_to_ptree(pt);
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    std::shared_ptr<ClassifierCreator> pCC = std::make_shared<ClassifierCreatorDispatchPtree>(pt);
    std::shared_ptr<Classifier> pClTmc = pCC->get_classifier();

    std::vector<double> p;  std::pair<double, double> conf;
    p = {10,2}; conf = {1.0/6.0,1.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {16,1}; conf = {1.0/3.0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {7,10}; conf = {2.0/3.0,2.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {11,10}; conf = {5.0/6.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {4,6}; conf = {0,1.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.second);
    p = {7,13}; conf = {5.0/6.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {14.5,0.5}; conf = {0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8)/(double)(50+10+16+8));
    p = {-1,16}; conf = {0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {-1,18}; conf = {0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {12.5,1.5}; conf = {1.0/6.0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8)/(double)(50+10+16+8));
    p = {4,8}; conf = {1.0/6.0,2.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18)/(double)(50+10+16+8+9+18));
    p = {3,13}; conf = {1.0/6.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {1,18};  conf = {1.0/6.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {15,5.5}; conf = {1.0/3.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (8+18+45)/(double)(16+8+9+18+9+45));
    p = {18,1}; conf = {1.0/3.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (8+18+45)/(double)(16+8+9+18+9+45));
    p = {8,12}; conf = {2.0/3.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (18+45)/(double)(9+18+9+45));
}

BOOST_AUTO_TEST_CASE( transformed_features )
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

    auto pFT = std::make_shared<FeatureTransformSubset>(boost::dynamic_bitset<>(std::string("10")));
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

    boost::property_tree::ptree pt;
    pClTrF->dump_to_ptree(pt);
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    std::shared_ptr<ClassifierCreator> pCC = std::make_shared<ClassifierCreatorDispatchPtree>(pt);
    std::shared_ptr<Classifier> pClTrF_from_ptree = pCC->get_classifier();

    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,0}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,1}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,2}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,3}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,4}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,5}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,6}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,7}), 0.32911392405063289);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,8}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,9}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,10}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,11}), 0.63492063492063489);
    BOOST_CHECK_EQUAL(pClTrF_from_ptree->confidence({123,12}), 0.63492063492063489);
}

BOOST_AUTO_TEST_SUITE_END()