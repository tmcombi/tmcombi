#define BOOST_TEST_MODULE lib_test_induced_graph
#include <boost/test/included/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "sample_creator.h"
#include "induced_graph.h"

BOOST_AUTO_TEST_CASE( induced_graph_basics ) {
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

    auto pInducedGraph = std::make_shared<InducedGraph<SampleGraphType, AuxTrGraphType> >(pSample);

    BOOST_CHECK_EQUAL(pSample->dim(), pInducedGraph->dim());
    BOOST_CHECK_EQUAL(pSample->size(), pInducedGraph->size());

    BOOST_CHECK_EQUAL(pInducedGraph->num_edges(), 28);

    pInducedGraph->print();

    pInducedGraph->do_transitive_reduction();
    BOOST_CHECK_EQUAL(pInducedGraph->num_edges(), 15);

    pInducedGraph->print();
}
