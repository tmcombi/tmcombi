#define BOOST_TEST_MODULE lib_test_layer_partitioner
#include <boost/test/included/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "sample_creator.h"
#include "graph_creator.h"
#include "layer_partitioner.h"

BOOST_AUTO_TEST_CASE( layer_partitioner_tmc_9 ) {
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


    typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
            boost::no_property,
            boost::property<boost::edge_capacity_t, double,
                    boost::property<boost::edge_residual_capacity_t, double,
                            boost::property<boost::edge_reverse_t, Traits::edge_descriptor> > > > SampleGraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    auto pGraphCreator = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pGraphCreator->size());
    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 5);
    BOOST_TEST_MESSAGE("Full induced graph:");
    pGraphCreator->print();
    pGraphCreator->do_transitive_reduction();
    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 4);
    BOOST_TEST_MESSAGE("TR-Reduced graph:");
    pGraphCreator->print();

    auto pLayerPartitioner = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner->set_layer(pSample);
    pLayerPartitioner->set_graph(pGraphCreator->get_graph());

    boost::dynamic_bitset<> mask;
    bool decomposable;
    std::tie(mask, decomposable) = pLayerPartitioner->compute();
    BOOST_CHECK(decomposable);
    BOOST_CHECK_EQUAL(mask, boost::dynamic_bitset<>(std::string("1010")));
    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 4);

}

BOOST_AUTO_TEST_CASE( layer_partitioner_36points ) {
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


    typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
            boost::property<boost::vertex_name_t, std::string>,
            boost::property<boost::edge_capacity_t, double,
                    boost::property<boost::edge_residual_capacity_t, double,
                            boost::property<boost::edge_reverse_t, Traits::edge_descriptor> > > > SampleGraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    auto pGraphCreator = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pGraphCreator->size());
    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 197);
    BOOST_TEST_MESSAGE("Full induced graph:");
    pGraphCreator->print();
    pGraphCreator->do_transitive_reduction();
    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 69);
    BOOST_TEST_MESSAGE("TR-Reduced graph:");
    pGraphCreator->print();

    auto pLayerPartitioner = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner->set_layer(pSample);
    pLayerPartitioner->set_graph(pGraphCreator->get_graph());

    boost::dynamic_bitset<> mask;
    bool decomposable;
    std::tie(mask, decomposable) = pLayerPartitioner->compute();
    BOOST_CHECK(decomposable);
    BOOST_CHECK_EQUAL(mask, boost::dynamic_bitset<>(std::string("111111111111111110000000000000000000")));
    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 69);

    std::shared_ptr<Layer> pLayer0;
    std::shared_ptr<Layer> pLayer1;
    std::tie(pLayer0,pLayer1) = sample_creator.split_sample(pSample, mask);
    BOOST_CHECK_EQUAL(pLayer0->size(), 19);
    BOOST_CHECK_EQUAL(pLayer1->size(), 17);

    auto pGraphCreator0 = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pLayer0);
    BOOST_CHECK_EQUAL(pLayer0->size(), pGraphCreator0->size());
    pGraphCreator0->do_transitive_reduction();
    auto pLayerPartitioner0 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner0->set_layer(pLayer0);
    pLayerPartitioner0->set_graph(pGraphCreator0->get_graph());
    boost::dynamic_bitset<> mask0;
    bool decomposable0;
    std::tie(mask0, decomposable0) = pLayerPartitioner0->compute();
    BOOST_CHECK(decomposable0);
    BOOST_CHECK_EQUAL(mask0, boost::dynamic_bitset<>(std::string("1111111000000000000")));
    std::shared_ptr<Layer> pLayer00;
    std::shared_ptr<Layer> pLayer01;
    std::tie(pLayer00,pLayer01) = sample_creator.split_sample(pLayer0, mask0);


    auto pGraphCreator1 = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pLayer1);
    BOOST_CHECK_EQUAL(pLayer1->size(), pGraphCreator1->size());
    pGraphCreator1->do_transitive_reduction();
    auto pLayerPartitioner1 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner1->set_layer(pLayer1);
    pLayerPartitioner1->set_graph(pGraphCreator1->get_graph());
    boost::dynamic_bitset<> mask1;
    bool decomposable1;
    std::tie(mask1, decomposable1) = pLayerPartitioner1->compute();
    BOOST_CHECK(decomposable1);
    BOOST_CHECK_EQUAL(mask1, boost::dynamic_bitset<>(std::string("11111111100000000")));
    std::shared_ptr<Layer> pLayer10;
    std::shared_ptr<Layer> pLayer11;
    std::tie(pLayer10,pLayer11) = sample_creator.split_sample(pLayer1, mask1);


    auto pGraphCreator00 = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pLayer00);
    BOOST_CHECK_EQUAL(pLayer00->size(), pGraphCreator00->size());
    pGraphCreator00->do_transitive_reduction();
    auto pLayerPartitioner00 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner00->set_layer(pLayer00);
    pLayerPartitioner00->set_graph(pGraphCreator00->get_graph());
    boost::dynamic_bitset<> mask00;
    bool decomposable00;
    std::tie(mask00, decomposable00) = pLayerPartitioner00->compute();
    BOOST_CHECK(!decomposable00);
    BOOST_CHECK_EQUAL(mask00, boost::dynamic_bitset<>(std::string("000000000000")));

    auto pGraphCreator01 = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pLayer01);
    BOOST_CHECK_EQUAL(pLayer01->size(), pGraphCreator01->size());
    pGraphCreator01->do_transitive_reduction();
    auto pLayerPartitioner01 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner01->set_layer(pLayer01);
    pLayerPartitioner01->set_graph(pGraphCreator01->get_graph());
    boost::dynamic_bitset<> mask01;
    bool decomposable01;
    std::tie(mask01, decomposable01) = pLayerPartitioner01->compute();
    BOOST_CHECK(!decomposable01);
    BOOST_CHECK_EQUAL(mask01, boost::dynamic_bitset<>(std::string("0000000")));

    auto pGraphCreator10 = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pLayer10);
    BOOST_CHECK_EQUAL(pLayer10->size(), pGraphCreator10->size());
    pGraphCreator10->do_transitive_reduction();
    auto pLayerPartitioner10 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner10->set_layer(pLayer10);
    pLayerPartitioner10->set_graph(pGraphCreator10->get_graph());
    boost::dynamic_bitset<> mask10;
    bool decomposable10;
    std::tie(mask10, decomposable10) = pLayerPartitioner10->compute();
    BOOST_CHECK(!decomposable10);
    BOOST_CHECK_EQUAL(mask10, boost::dynamic_bitset<>(std::string("00000000")));

    auto pGraphCreator11 = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pLayer11);
    BOOST_CHECK_EQUAL(pLayer11->size(), pGraphCreator11->size());
    pGraphCreator11->do_transitive_reduction();
    auto pLayerPartitioner11 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner11->set_layer(pLayer11);
    pLayerPartitioner11->set_graph(pGraphCreator11->get_graph());
    boost::dynamic_bitset<> mask11;
    bool decomposable11;
    std::tie(mask11, decomposable11) = pLayerPartitioner11->compute();
    BOOST_CHECK(!decomposable11);
    BOOST_CHECK_EQUAL(mask11, boost::dynamic_bitset<>(std::string("000000000")));
}
