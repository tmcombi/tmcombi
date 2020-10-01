#define BOOST_TEST_MODULE lib_test_border_system
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"
#include "border_system.h"

BOOST_AUTO_TEST_CASE( border_system_from_36points_layer_partitioning ) {
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

    std::shared_ptr<LayerPartitioning> pLD = std::make_shared<LayerPartitioning>(pSample);

    BOOST_TEST_MESSAGE("Create layer partitioning via splitting the lowest layer three times into 4 layers");
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

    std::shared_ptr<BorderSystem> pBS = std::make_shared<BorderSystem>(pLD);
    BOOST_CHECK_EQUAL(pBS->get_dim(), pLD->get_dim());
    BOOST_CHECK_EQUAL(pBS->get_size(), pLD->get_size());
    BOOST_CHECK(pBS->consistent());
    const Border & LowerBorder0 = *pBS->get_lower(0);
    const Border & LowerBorder1 = *pBS->get_lower(1);
    const Border & LowerBorder2 = *pBS->get_lower(2);
    const Border & LowerBorder3 = *pBS->get_lower(3);

    const Border & UpperBorder0 = *pBS->get_upper(0);
    const Border & UpperBorder1 = *pBS->get_upper(1);
    const Border & UpperBorder2 = *pBS->get_upper(2);
    const Border & UpperBorder3 = *pBS->get_upper(3);

    BOOST_TEST_MESSAGE("First lower border: " << LowerBorder0);
    BOOST_TEST_MESSAGE("Second lower border: " << LowerBorder1);
    BOOST_TEST_MESSAGE("Third lower border: " << LowerBorder2);
    BOOST_TEST_MESSAGE("Fourth lower border: " << LowerBorder3);

    BOOST_TEST_MESSAGE("First upper border: " << UpperBorder0);
    BOOST_TEST_MESSAGE("Second upper border: " << UpperBorder1);
    BOOST_TEST_MESSAGE("Third upper border: " << UpperBorder2);
    BOOST_TEST_MESSAGE("Fourth upper border: " << UpperBorder3);

    BOOST_CHECK(pLayer1->get_neg_pos_counts() == LowerBorder0.get_neg_pos_counts());
    BOOST_CHECK(pLayer1->get_neg_pos_counts() == UpperBorder0.get_neg_pos_counts());
    BOOST_CHECK(pLayer2->get_neg_pos_counts() == LowerBorder1.get_neg_pos_counts());
    BOOST_CHECK(pLayer2->get_neg_pos_counts() == UpperBorder1.get_neg_pos_counts());
    BOOST_CHECK(pLayer3->get_neg_pos_counts() == LowerBorder2.get_neg_pos_counts());
    BOOST_CHECK(pLayer3->get_neg_pos_counts() == UpperBorder2.get_neg_pos_counts());
    BOOST_CHECK(pLayer4->get_neg_pos_counts() == LowerBorder3.get_neg_pos_counts());
    BOOST_CHECK(pLayer4->get_neg_pos_counts() == UpperBorder3.get_neg_pos_counts());

    BOOST_CHECK_EQUAL(LowerBorder0.get_size(), 5);
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{0,11})));
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{2,7})));
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{5,4})));
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{8,3})));
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{9,1})));

    BOOST_CHECK_EQUAL(LowerBorder1.get_size(), 5);
    BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{0,11})));
    BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{2,7})));
    BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{15,0})));
    BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{13,2})));
    BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{12,5})));

    BOOST_CHECK_EQUAL(LowerBorder3.get_size(), 5);
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{16,6})));
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{13,7})));
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{9,9})));
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{6,13})));
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{3,14})));




}

BOOST_AUTO_TEST_CASE( border_system_ptree ) {
    //todo: implement
}

BOOST_AUTO_TEST_CASE( border_system_containing_border ) {
    //todo: implement check slow vs. fast
}

