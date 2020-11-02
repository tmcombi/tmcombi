#define BOOST_TEST_MODULE lib_test_evaluator
#include <boost/test/included/unit_test.hpp>

#include "border_system_creator.h"
#include "evaluator.h"
#include "layer_partitioning.h"
#include "sample_creator.h"


BOOST_AUTO_TEST_CASE( evaluator_basics ) {
    const std::string names_file("data/4layers_36points/4layers_36points.names");
    const std::string data_file("data/4layers_36points/4layers_36points.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);

    boost::dynamic_bitset<> db4(36);
    boost::dynamic_bitset<> db3(27);
    boost::dynamic_bitset<> db2(19);
    db4[27] = db4[28] = db4[29] = db4[30] = db4[31] = db4[32] = db4[33] = db4[34] = db4[35] = true;
    db3[19] = db3[20] = db3[21] = db3[22] = db3[23] = db3[24] = db3[25] = db3[26] = true;
    db2[12] = db2[13] = db2[14] = db2[15] = db2[16] = db2[17] = db2[18] = true;

    std::shared_ptr<LayerPartitioning> pLD = std::make_shared<LayerPartitioning>();
    pLD->push_back(pSample);

    BOOST_TEST_MESSAGE("Create layer partitioning via splitting the lowest layer three times into 4 layers");
    { auto it = pLD->begin(); pLD->split_layer(it, db4); }
    { auto it = pLD->begin(); pLD->split_layer(it, db3); }
    { auto it = pLD->begin(); pLD->split_layer(it, db2); }

    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    auto pLayer1 = *pLD->begin();
    auto pLayer2 = *(pLD->begin() + 1);
    auto pLayer3 = *(pLD->begin() + 2);
    auto pLayer4 = *(pLD->begin() + 3);

    BOOST_TEST_MESSAGE("Creating border system");
    std::shared_ptr<BorderSystemCreator> pBSC = std::make_shared<BorderSystemCreator>();
    std::shared_ptr<BorderSystem> pBS = pBSC->from_layer_partitioning(pLD);
    BOOST_CHECK_EQUAL(pBS.use_count(), 1);
    std::vector<double> p;  std::pair<int, int> borders; std::pair<double, double> conf;
    p = {10,2}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {10,2}; conf = {1.0/6.0,1.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);

    auto pEvaluator = std::make_shared<Evaluator>();
    auto pSample2Evaluate = std::make_shared<Sample>(2);
    (*pEvaluator).set_border_system(pBS).set_sample(pSample2Evaluate);
    pSample2Evaluate->push((*pSample)[0]);
}
