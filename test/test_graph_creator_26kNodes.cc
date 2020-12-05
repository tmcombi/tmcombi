#define BOOST_TEST_MODULE test_graph_creator_26kNodes
#include <boost/test/included/unit_test.hpp>

#define TIMERS

#include "../lib/sample_creator.h"
#include "../lib/graph_creator.h"

BOOST_AUTO_TEST_CASE( adult_transformed_graph_creator ) {
    const std::string names_file("data/adult_transformed/adult_transformed.names");
    const std::string data_file("data/adult_transformed/adult_transformed.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 13);
    BOOST_CHECK_EQUAL(pSample->size(), 26314);

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> SampleGraphType;
    typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS> AuxTrGraphType;

    auto pGraphCreator = std::make_shared<GraphCreator<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->size(), pGraphCreator->size());

    BOOST_TEST_MESSAGE("Num edges in the induced graph: " << pGraphCreator->num_edges());

    BOOST_CHECK_EQUAL(pGraphCreator->num_edges(), 417857);

    //pGraphCreator->print();
    //{ std::ofstream os("reduced.dot"); boost::write_graphviz(os, *pGraphCreator->get_graph()); os.close(); }
}
