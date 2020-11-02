#define BOOST_TEST_MODULE test_tmc_paper_dataset
#include <boost/test/included/unit_test.hpp>

#define TIMERS

#include "../lib/sample_creator.h"
#include "../lib/layer_partitioning_creator.h"
//#include "../lib/border_system_creator.h"

BOOST_AUTO_TEST_CASE( tmc_paper_dataset_do_one_step ) {
    const std::string names_file("data/tmc_paper/tmc_paper.names");
    const std::string data_file("data/tmc_paper/tmc_paper.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample size: " << pSample->size());
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 1);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
    pLayerPartitioningCreator->do_one_step();
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 2);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
    pLayerPartitioningCreator->do_one_step();
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 3);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
    pLayerPartitioningCreator->do_one_step();
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 4);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
}

BOOST_AUTO_TEST_CASE( tmc_paper_dataset_optimize ) {
    const std::string names_file("data/tmc_paper/tmc_paper.names");
    const std::string data_file("data/tmc_paper/tmc_paper.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample size: " << pSample->size());
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);

    pLayerPartitioningCreator->optimize();
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 29);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
}