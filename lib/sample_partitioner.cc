#define BOOST_TEST_MODULE lib_test_sample_partitioner
#include <boost/test/included/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "sample_creator.h"
#include "sample_partitioner.h"


BOOST_AUTO_TEST_CASE( sample_partitioner_tmc_9 ) {
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

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS> SampleGraphType;
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

    auto pSamplePartitioner = std::make_shared<SamplePartitioner<SampleGraphType> >();
    pSamplePartitioner->set_sample(pSample);
    pSamplePartitioner->set_graph(pInducedGraph->get_graph());

    boost::dynamic_bitset<> marks;
    bool decomposable;
    std::tie(marks, decomposable) = pSamplePartitioner->compute();
    BOOST_CHECK(decomposable);
    BOOST_CHECK_EQUAL(marks, boost::dynamic_bitset<>(std::string("1010")));

/*
    glp_prob *lp;
    lp = glp_create_prob();
    glp_delete_prob(lp);
    */
}
