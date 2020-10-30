#define BOOST_TEST_MODULE lib_test_layer_partitioning
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"
#include "layer_partitioning_creator.h"

BOOST_AUTO_TEST_CASE( layer_partitioning_creator_tmc_9 ) {
    const std::string names_file("data/tmc_paper_9/tmc_paper.names");
    const std::string data_file("data/tmc_paper_9/tmc_paper.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 4);

    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);
    pLayerPartitioningCreator->optimize();
    std::shared_ptr<LayerPartitioning> pLayerPartitioning(pLayerPartitioningCreator->get_layer_partitioning());
    BOOST_CHECK_EQUAL(pLayerPartitioning->size(), 2);
}

BOOST_AUTO_TEST_CASE( layer_partitioning_creator_36points_example ) {
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
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(),true);
    pLayerPartitioningCreator->optimize();
    std::shared_ptr<LayerPartitioning> pLD(pLayerPartitioningCreator->get_layer_partitioning());
    BOOST_CHECK_EQUAL(pLD->size(), 4);


    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    auto pLayer1 = *pLD->begin();
    auto pLayer2 = *(pLD->begin() + 1);
    auto pLayer3 = *(pLD->begin() + 2);
    auto pLayer4 = *(pLD->begin() + 3);

    BOOST_TEST_MESSAGE("First layer: " << *pLayer1);
    BOOST_TEST_MESSAGE("Second layer: " << *pLayer2);
    BOOST_TEST_MESSAGE("Third layer: " << *pLayer3);
    BOOST_TEST_MESSAGE("Fourth layer: " << *pLayer4);

    BOOST_CHECK_LE(*pLayer1, *pLayer2);
    BOOST_CHECK_LE(*pLayer1, *pLayer3);
    BOOST_CHECK_LE(*pLayer1, *pLayer4);
    BOOST_CHECK_LE(*pLayer2, *pLayer3);
    BOOST_CHECK_LE(*pLayer2, *pLayer4);
    BOOST_CHECK_LE(*pLayer3, *pLayer4);
    BOOST_CHECK_GE(*pLayer4, *pLayer3);
    BOOST_CHECK_GE(*pLayer3, *pLayer2);
    BOOST_CHECK_GE(*pLayer2, *pLayer1);

    BOOST_CHECK_EQUAL(pLayer1->get_neg_pos_counts().second / pLayer1->get_neg_pos_counts().first, 1.0/5.0 );
    BOOST_CHECK_EQUAL(pLayer2->get_neg_pos_counts().second / pLayer2->get_neg_pos_counts().first, 1.0/2.0 );
    BOOST_CHECK_EQUAL(pLayer3->get_neg_pos_counts().second / pLayer3->get_neg_pos_counts().first, 2.0/1.0 );
    BOOST_CHECK_EQUAL(pLayer4->get_neg_pos_counts().second / pLayer4->get_neg_pos_counts().first, 5.0/1.0 );

    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer1)), pLayer1->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer2)), pLayer2->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer3)), pLayer3->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer4)), pLayer4->size());

    pLD->merge_two_layers(pLD->begin()+1);
    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    pLD->merge_two_layers(pLD->begin()+1);
    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    pLD->merge_two_layers(pLD->begin());
    BOOST_CHECK_EQUAL(pLD->consistent(),true);

    BOOST_CHECK_EQUAL(36, boost::num_vertices(*pLD->get_graph(*pLD->begin())));
}

BOOST_AUTO_TEST_CASE( layer_partitioning_creator_36points_enforce_merge ) {
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

    boost::dynamic_bitset<> db4(36);
    db4[25] = db4[27] = db4[28] = db4[29] = db4[30] = db4[31] = db4[32] = db4[33] = db4[34] = db4[35] = true;

    std::shared_ptr<LayerPartitioning> pLD_aux = std::make_shared<LayerPartitioning>();
    pLD_aux->push_back(pSample);

    BOOST_TEST_MESSAGE("Create layer partitioning via splitting the lowest layer three times into 4 layers");
    auto it_aux = pLD_aux->begin();
    pLD_aux->split_layer(it_aux, db4);
    BOOST_CHECK_EQUAL(pLD_aux->consistent(),true);

    auto pSample1 = *pLD_aux->begin();
    auto pSample2 = *(pLD_aux->begin()+1);

    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample1);
    pLayerPartitioningCreator->push_back(pSample2);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 2);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(),true);
    pLayerPartitioningCreator->optimize();
    std::shared_ptr<LayerPartitioning> pLD(pLayerPartitioningCreator->get_layer_partitioning());
    BOOST_CHECK_EQUAL(pLD->size(), 4);


    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    auto pLayer1 = *pLD->begin();
    auto pLayer2 = *(pLD->begin() + 1);
    auto pLayer3 = *(pLD->begin() + 2);
    auto pLayer4 = *(pLD->begin() + 3);

    BOOST_TEST_MESSAGE("First layer: " << *pLayer1);
    BOOST_TEST_MESSAGE("Second layer: " << *pLayer2);
    BOOST_TEST_MESSAGE("Third layer: " << *pLayer3);
    BOOST_TEST_MESSAGE("Fourth layer: " << *pLayer4);

    BOOST_CHECK_LE(*pLayer1, *pLayer2);
    BOOST_CHECK_LE(*pLayer1, *pLayer3);
    BOOST_CHECK_LE(*pLayer1, *pLayer4);
    BOOST_CHECK_LE(*pLayer2, *pLayer3);
    BOOST_CHECK_LE(*pLayer2, *pLayer4);
    BOOST_CHECK_LE(*pLayer3, *pLayer4);
    BOOST_CHECK_GE(*pLayer4, *pLayer3);
    BOOST_CHECK_GE(*pLayer3, *pLayer2);
    BOOST_CHECK_GE(*pLayer2, *pLayer1);

    BOOST_CHECK_EQUAL(pLayer1->get_neg_pos_counts().second / pLayer1->get_neg_pos_counts().first, 1.0/5.0 );
    BOOST_CHECK_EQUAL(pLayer2->get_neg_pos_counts().second / pLayer2->get_neg_pos_counts().first, 1.0/2.0 );
    BOOST_CHECK_EQUAL(pLayer3->get_neg_pos_counts().second / pLayer3->get_neg_pos_counts().first, 2.0/1.0 );
    BOOST_CHECK_EQUAL(pLayer4->get_neg_pos_counts().second / pLayer4->get_neg_pos_counts().first, 5.0/1.0 );

    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer1)), pLayer1->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer2)), pLayer2->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer3)), pLayer3->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer4)), pLayer4->size());

    pLD->merge_two_layers(pLD->begin()+1);
    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    pLD->merge_two_layers(pLD->begin()+1);
    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    pLD->merge_two_layers(pLD->begin());
    BOOST_CHECK_EQUAL(pLD->consistent(),true);

    BOOST_CHECK_EQUAL(36, boost::num_vertices(*pLD->get_graph(*pLD->begin())));
}
