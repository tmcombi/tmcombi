#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_classifier_tmc
#endif
#include <boost/test/included/unit_test.hpp>

#include "border_system_creator.h"
#include "classifier_tmc.h"
#include "feature_names.h"
#include "layer_partitioning_creator.h"
#include "sample_creator.h"

BOOST_AUTO_TEST_SUITE( classifier_tmc )

BOOST_AUTO_TEST_CASE( basics )
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

    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 1);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
    pLayerPartitioningCreator->optimize();
    std::shared_ptr<LayerPartitioning> pLD(pLayerPartitioningCreator->get_layer_partitioning());
    BOOST_CHECK_EQUAL(pLD->size(), 4);
    BOOST_CHECK_EQUAL(pLD->consistent(), true);
    auto pLayer1 = *pLD->begin();
    auto pLayer2 = *(pLD->begin() + 1);
    auto pLayer3 = *(pLD->begin() + 2);
    auto pLayer4 = *(pLD->begin() + 3);

    std::shared_ptr<BorderSystemCreator> pBSC = std::make_shared<BorderSystemCreator>();
    std::shared_ptr<BorderSystem> pBS = pBSC->from_layer_partitioning(pLD);
    BOOST_CHECK_EQUAL(pBS->dim(), pLD->dim());
    BOOST_CHECK_EQUAL(pBS->size(), pLD->size());
    BOOST_CHECK(pBS->consistent());

    auto pClTmc = std::make_shared<ClassifierTmc>(pBS);

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

BOOST_AUTO_TEST_SUITE_END()