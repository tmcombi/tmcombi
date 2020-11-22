#define BOOST_TEST_MODULE lib_test_graph_creator
#include <boost/test/included/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/graphviz.hpp>

#include "sample_creator.h"
#include "graph_creator.h"

BOOST_AUTO_TEST_CASE( graph_creator_12points ) {
    std::string names_buffer("| this is comment\n"
                             "target_feature.| one more comment\n"
                             "\n"
                             "   feature1: continuous.   \n"
                             "feature2: continuous.   | third comment\n"
                             "feature3: ignore.\n"
                             "feature4: ignore.\n"
                             "target_feature: v1, v2.\n"
                             "case weight: continuous.\n"
                             "feature5: ignore.\n"
                             "\n"
                             "  | one more comment here\n"
                             "\n");
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);
    std::string data_buffer("1,8,34,44,v2,1,77\n"
                            "2,2,34,44,v1,2,77\n"
                            "2,5,34,44,v2,3,77\n"
                            "3,7,34,44,v2,1,77\n"
                            "3,9,34,44,v1,2,77\n"
                            "4,1,34,44,v2,3,77\n"
                            "4,5,34,44,v2,1,77\n"
                            "5,8,34,44,v1,2,77\n"
                            "7,1,34,44,v2,3,77\n"
                            "6,0,34,44,v2,3,77\n"
                            "7,5,34,44,v2,3,77\n"
                            "8,2,34,44,v2,3,77\n"
    );
    std::stringstream ss_buffer((std::stringstream(data_buffer)));

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_stream(ss_buffer);
    BOOST_TEST_MESSAGE("Sample: " << *pSample);

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> SampleGraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    auto pGraphCreator = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pGraphCreator->size());

    //BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 28);

    BOOST_TEST_MESSAGE("Full induced graph:");
    pGraphCreator->print();
    // { std::ofstream os("full.dot"); boost::write_graphviz(os, *pGraphCreator->get_graph()); os.close(); }

    //pGraphCreator->do_transitive_reduction();
    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 15);

    BOOST_TEST_MESSAGE("TR-Reduced graph:");
    pGraphCreator->print();
    // { std::ofstream os("reduced.dot"); boost::write_graphviz(os, *pGraphCreator->get_graph()); os.close(); }
}

BOOST_AUTO_TEST_CASE( graph_creator_36points ) {
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

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> SampleGraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    auto pGraphCreator = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pGraphCreator->size());

    BOOST_TEST_MESSAGE("Num edges in the full induced graph: " << pGraphCreator->num_edges());
    //pGraphCreator->print();
    //{ std::ofstream os("full.dot"); boost::write_graphviz(os, *pGraphCreator->get_graph()); os.close(); }

    //pGraphCreator->do_transitive_reduction();

    BOOST_TEST_MESSAGE("Num edges in the TR-reduced graph: " << pGraphCreator->num_edges());

    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 69);

    //pGraphCreator->print();
    //{ std::ofstream os("reduced.dot"); boost::write_graphviz(os, *pGraphCreator->get_graph()); os.close(); }
}


BOOST_AUTO_TEST_CASE( graph_creator_from_graph_12points ) {
    std::string names_buffer("| this is comment\n"
                             "target_feature.| one more comment\n"
                             "\n"
                             "   feature1: continuous.   \n"
                             "feature2: continuous.   | third comment\n"
                             "feature3: ignore.\n"
                             "feature4: ignore.\n"
                             "target_feature: v1, v2.\n"
                             "case weight: continuous.\n"
                             "feature5: ignore.\n"
                             "\n"
                             "  | one more comment here\n"
                             "\n");
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);
    std::string data_buffer("1,8,34,44,v2,1,77\n"
                            "2,2,34,44,v1,2,77\n"
                            "2,5,34,44,v2,3,77\n"
                            "3,7,34,44,v2,1,77\n"
                            "3,9,34,44,v1,2,77\n"
                            "4,1,34,44,v2,3,77\n"
                            "4,5,34,44,v2,1,77\n"
                            "5,8,34,44,v1,2,77\n"
                            "7,1,34,44,v2,3,77\n"
                            "6,0,34,44,v2,3,77\n"
                            "7,5,34,44,v2,3,77\n"
                            "8,2,34,44,v2,3,77\n"
    );
    std::stringstream ss_buffer((std::stringstream(data_buffer)));

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_stream(ss_buffer);
    BOOST_TEST_MESSAGE("Sample: " << *pSample);

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> SampleGraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    auto pGraphCreator = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pGraphCreator->size());
    //BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 28);
    //pGraphCreator->do_transitive_reduction();
    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 15);

    BOOST_TEST_MESSAGE("Base graph:");
    pGraphCreator->print();

    boost::dynamic_bitset<> bs0(std::string("000010011001"));
    boost::dynamic_bitset<> bs1 = bs0; bs1.flip();

    auto pSubGraph0 = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pGraphCreator->get_graph(), bs0);
    auto pSubGraph1 = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pGraphCreator->get_graph(), bs1);

    BOOST_CHECK_EQUAL(pSubGraph0->size(),4);
    BOOST_CHECK_EQUAL(pSubGraph1->size(),8);

    BOOST_CHECK_EQUAL(pSubGraph0->num_edges(),4);
    BOOST_CHECK_EQUAL(pSubGraph1->num_edges(),9);

    BOOST_TEST_MESSAGE("Subgraph 0:");
    pSubGraph0->print();
    //{ std::ofstream os("subgraph0.dot"); boost::write_graphviz(os, *pSubGraph0->get_graph()); os.close(); }

    BOOST_TEST_MESSAGE("Subgraph 1:");
    pSubGraph1->print();
    //{ std::ofstream os("subgraph1.dot"); boost::write_graphviz(os, *pSubGraph1->get_graph()); os.close(); }
}

BOOST_AUTO_TEST_CASE( graph_creator_tmc_paper_dataset ) {
    const std::string names_file("data/tmc_paper/tmc_paper.names");
    const std::string data_file("data/tmc_paper/tmc_paper.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> SampleGraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    auto pGraphCreator = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pGraphCreator->size());

    BOOST_TEST_MESSAGE("Num edges in the induced graph: " << pGraphCreator->num_edges());

    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 5188);

    //pGraphCreator->print();
    //{ std::ofstream os("reduced.dot"); boost::write_graphviz(os, *pGraphCreator->get_graph()); os.close(); }
}
