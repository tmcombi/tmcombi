#define BOOST_TEST_MODULE lib_test_border_system
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"
#include "border_system.h"

BOOST_AUTO_TEST_CASE( border_system_basics ) {
    //todo: implement
}

BOOST_AUTO_TEST_CASE( border_system_ptree ) {
    //todo: implement
}

BOOST_AUTO_TEST_CASE( border_system_36points_example ) {
    BOOST_TEST_MESSAGE("More comprehensive example");
    const std::string names_file("data/4layers_36points/4layers_36points.names");
    const std::string data_file("data/4layers_36points/4layers_36points.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->get_dim(), 2);
    BOOST_CHECK_EQUAL(pSample->get_size(), 36);

    boost::dynamic_bitset<> db4(36);
    boost::dynamic_bitset<> db3(27);
    boost::dynamic_bitset<> db2(19);
    db4[27] = db4[28] = db4[29] = db4[30] = db4[31] = db4[32] = db4[33] = db4[34] = db4[35] = true;
    db3[19] = db3[20] = db3[21] = db3[22] = db3[23] = db3[24] = db3[25] = db3[26] = true;
    db2[12] = db2[13] = db2[14] = db2[15] = db2[16] = db2[17] = db2[18] = true;

    std::shared_ptr<LayerPartitioning> pLD = std::make_shared<LayerPartitioning>(pSample);

    BOOST_TEST_MESSAGE("Splitting the lowest layer three times into 4 layers");
    pLD->split_layer(pLD->begin(), db4);
    pLD->split_layer(pLD->begin(), db3);
    pLD->split_layer(pLD->begin(), db2);

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
}

BOOST_AUTO_TEST_CASE( border_system_containing_border ) {
    //todo: implement check slow vs. fast
}

