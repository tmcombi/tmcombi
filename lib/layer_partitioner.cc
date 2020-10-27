#define BOOST_TEST_MODULE lib_test_layer_partitioner
#include <boost/test/included/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/read_dimacs.hpp>

#include "sample_creator.h"
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
            boost::property<boost::vertex_name_t, std::string>,
            boost::property<boost::edge_capacity_t, double,
                    boost::property<boost::edge_residual_capacity_t, double,
                            boost::property<boost::edge_reverse_t, Traits::edge_descriptor> > > > SampleGraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    auto pInducedGraph = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pInducedGraph->size());
    BOOST_CHECK_EQUAL(pInducedGraph->num_edges(), 5);
    BOOST_TEST_MESSAGE("Full induced graph:");
    pInducedGraph->print();
    pInducedGraph->do_transitive_reduction();
    BOOST_CHECK_EQUAL(pInducedGraph->num_edges(), 4);
    BOOST_TEST_MESSAGE("TR-Reduced graph:");
    pInducedGraph->print();

    auto pLayerPartitioner = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner->set_layer(pSample);
    pLayerPartitioner->set_graph(pInducedGraph->get_graph());

    boost::dynamic_bitset<> marks;
    bool decomposable;
    std::tie(marks, decomposable) = pLayerPartitioner->compute();
    BOOST_CHECK(decomposable);
    BOOST_CHECK_EQUAL(marks, boost::dynamic_bitset<>(std::string("1010")));
    BOOST_CHECK_EQUAL(pInducedGraph->num_edges(), 4);

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

    auto pInducedGraph = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pInducedGraph->size());
    BOOST_CHECK_EQUAL(pInducedGraph->num_edges(), 197);
    BOOST_TEST_MESSAGE("Full induced graph:");
    pInducedGraph->print();
    pInducedGraph->do_transitive_reduction();
    BOOST_CHECK_EQUAL(pInducedGraph->num_edges(), 69);
    BOOST_TEST_MESSAGE("TR-Reduced graph:");
    pInducedGraph->print();

    auto pLayerPartitioner = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner->set_layer(pSample);
    pLayerPartitioner->set_graph(pInducedGraph->get_graph());

    boost::dynamic_bitset<> marks;
    bool decomposable;
    std::tie(marks, decomposable) = pLayerPartitioner->compute();
    BOOST_CHECK(decomposable);
    BOOST_CHECK_EQUAL(marks, boost::dynamic_bitset<>(std::string("111111111111111110000000000000000000")));
    BOOST_CHECK_EQUAL(pInducedGraph->num_edges(), 69);

    std::shared_ptr<Layer> pLayer0;
    std::shared_ptr<Layer> pLayer1;
    std::tie(pLayer0,pLayer1) = sample_creator.split_sample(pSample, marks);
    BOOST_CHECK_EQUAL(pLayer0->size(), 19);
    BOOST_CHECK_EQUAL(pLayer1->size(), 17);

    auto pInducedGraph0 = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pLayer0);
    BOOST_CHECK_EQUAL(pLayer0->size(), pInducedGraph0->size());
    pInducedGraph0->do_transitive_reduction();
    auto pLayerPartitioner0 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner0->set_layer(pLayer0);
    pLayerPartitioner0->set_graph(pInducedGraph0->get_graph());
    boost::dynamic_bitset<> marks0;
    bool decomposable0;
    std::tie(marks0, decomposable0) = pLayerPartitioner0->compute();
    BOOST_CHECK(decomposable0);
    BOOST_CHECK_EQUAL(marks0, boost::dynamic_bitset<>(std::string("1111111000000000000")));
    std::shared_ptr<Layer> pLayer00;
    std::shared_ptr<Layer> pLayer01;
    std::tie(pLayer00,pLayer01) = sample_creator.split_sample(pLayer0, marks0);


    auto pInducedGraph1 = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pLayer1);
    BOOST_CHECK_EQUAL(pLayer1->size(), pInducedGraph1->size());
    pInducedGraph1->do_transitive_reduction();
    auto pLayerPartitioner1 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner1->set_layer(pLayer1);
    pLayerPartitioner1->set_graph(pInducedGraph1->get_graph());
    boost::dynamic_bitset<> marks1;
    bool decomposable1;
    std::tie(marks1, decomposable1) = pLayerPartitioner1->compute();
    BOOST_CHECK(decomposable1);
    BOOST_CHECK_EQUAL(marks1, boost::dynamic_bitset<>(std::string("11111111100000000")));
    std::shared_ptr<Layer> pLayer10;
    std::shared_ptr<Layer> pLayer11;
    std::tie(pLayer10,pLayer11) = sample_creator.split_sample(pLayer1, marks1);


    auto pInducedGraph00 = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pLayer00);
    BOOST_CHECK_EQUAL(pLayer00->size(), pInducedGraph00->size());
    pInducedGraph00->do_transitive_reduction();
    auto pLayerPartitioner00 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner00->set_layer(pLayer00);
    pLayerPartitioner00->set_graph(pInducedGraph00->get_graph());
    boost::dynamic_bitset<> marks00;
    bool decomposable00;
    std::tie(marks00, decomposable00) = pLayerPartitioner00->compute();
    BOOST_CHECK(!decomposable00);
    BOOST_CHECK_EQUAL(marks00, boost::dynamic_bitset<>(std::string("000000000000")));

    auto pInducedGraph01 = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pLayer01);
    BOOST_CHECK_EQUAL(pLayer01->size(), pInducedGraph01->size());
    pInducedGraph01->do_transitive_reduction();
    auto pLayerPartitioner01 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner01->set_layer(pLayer01);
    pLayerPartitioner01->set_graph(pInducedGraph01->get_graph());
    boost::dynamic_bitset<> marks01;
    bool decomposable01;
    std::tie(marks01, decomposable01) = pLayerPartitioner01->compute();
    BOOST_CHECK(!decomposable01);
    BOOST_CHECK_EQUAL(marks01, boost::dynamic_bitset<>(std::string("0000000")));

    auto pInducedGraph10 = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pLayer10);
    BOOST_CHECK_EQUAL(pLayer10->size(), pInducedGraph10->size());
    pInducedGraph10->do_transitive_reduction();
    auto pLayerPartitioner10 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner10->set_layer(pLayer10);
    pLayerPartitioner10->set_graph(pInducedGraph10->get_graph());
    boost::dynamic_bitset<> marks10;
    bool decomposable10;
    std::tie(marks10, decomposable10) = pLayerPartitioner10->compute();
    BOOST_CHECK(!decomposable10);
    BOOST_CHECK_EQUAL(marks10, boost::dynamic_bitset<>(std::string("00000000")));

    auto pInducedGraph11 = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pLayer11);
    BOOST_CHECK_EQUAL(pLayer11->size(), pInducedGraph11->size());
    pInducedGraph11->do_transitive_reduction();
    auto pLayerPartitioner11 = std::make_shared<LayerPartitioner<SampleGraphType> >();
    pLayerPartitioner11->set_layer(pLayer11);
    pLayerPartitioner11->set_graph(pInducedGraph11->get_graph());
    boost::dynamic_bitset<> marks11;
    bool decomposable11;
    std::tie(marks11, decomposable11) = pLayerPartitioner11->compute();
    BOOST_CHECK(!decomposable11);
    BOOST_CHECK_EQUAL(marks11, boost::dynamic_bitset<>(std::string("000000000")));
}
