#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_border_system
#endif
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "sample_creator.h"
#include "border_system_creator.h"

BOOST_AUTO_TEST_SUITE( border_system )

BOOST_AUTO_TEST_CASE( from_36points_layer_partitioning ) {
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

    BOOST_TEST_MESSAGE("First layer: " << *pLayer1);
    BOOST_TEST_MESSAGE("Second layer: " << *pLayer2);
    BOOST_TEST_MESSAGE("Third layer: " << *pLayer3);
    BOOST_TEST_MESSAGE("Fourth layer: " << *pLayer4);

    std::shared_ptr<BorderSystemCreator> pBSC = std::make_shared<BorderSystemCreator>();
    std::shared_ptr<BorderSystem> pBS = pBSC->from_layer_partitioning(pLD);
    BOOST_CHECK_EQUAL(pBS->dim(), pLD->dim());
    BOOST_CHECK_EQUAL(pBS->size(), pLD->size());
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

    //lower borders
    BOOST_CHECK_EQUAL(LowerBorder0.size(), 5);
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{9,1})));
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{8,3})));
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{5,4})));
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{2,7})));
    BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{0,11})));

    BOOST_CHECK_EQUAL(LowerBorder1.size(), 3);
    BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{15,0})));
    BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{13,2})));
    BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{12,5})));

    BOOST_CHECK_EQUAL(LowerBorder2.size(), 3);
    BOOST_CHECK(LowerBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{11,8})));
    BOOST_CHECK(LowerBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{5,9})));
    BOOST_CHECK(LowerBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{4,10})));

    BOOST_CHECK_EQUAL(LowerBorder3.size(), 5);
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{16,6})));
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{13,7})));
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{9,9})));
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{6,13})));
    BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{3,14})));

    // upper borders
    BOOST_CHECK_EQUAL(UpperBorder3.size(), 6);
    BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{16,6})));
    BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{13,7})));
    BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{12,11})));
    BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{9,12})));
    BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{6,15})));
    BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{5,17})));

    BOOST_CHECK_EQUAL(UpperBorder2.size(), 3);
    BOOST_CHECK(UpperBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{11,8})));
    BOOST_CHECK(UpperBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{8,11})));
    BOOST_CHECK(UpperBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{7,12})));

    BOOST_CHECK_EQUAL(UpperBorder1.size(), 2);
    BOOST_CHECK(UpperBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{17,5})));
    BOOST_CHECK(UpperBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{14,6})));

    BOOST_CHECK_EQUAL(UpperBorder0.size(), 5);
    BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{14,1})));
    BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{12,3})));
    BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{11,7})));
    BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{3,12})));
    BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{2,15})));
}

BOOST_AUTO_TEST_CASE( ptree ) {
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

    BOOST_TEST_MESSAGE("First layer: " << *pLayer1);
    BOOST_TEST_MESSAGE("Second layer: " << *pLayer2);
    BOOST_TEST_MESSAGE("Third layer: " << *pLayer3);
    BOOST_TEST_MESSAGE("Fourth layer: " << *pLayer4);

    std::shared_ptr<BorderSystemCreator> pBSC = std::make_shared<BorderSystemCreator>();
    std::shared_ptr<BorderSystem> pBS1 = pBSC->from_layer_partitioning(pLD);

    boost::property_tree::ptree pt;
    pBS1->dump_to_ptree(pt);
    BOOST_CHECK_EQUAL(pt.size(), 4);
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    std::shared_ptr<BorderSystem> pBS = std::make_shared<BorderSystem>(pt);
    BOOST_CHECK_EQUAL(pBS->dim(), pLD->dim());
    BOOST_CHECK_EQUAL(pBS->size(), pLD->size());
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

        //lower borders
        BOOST_CHECK_EQUAL(LowerBorder0.size(), 5);
        BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{9,1})));
        BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{8,3})));
        BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{5,4})));
        BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{2,7})));
        BOOST_CHECK(LowerBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{0,11})));

        BOOST_CHECK_EQUAL(LowerBorder1.size(), 3);
        BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{15,0})));
        BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{13,2})));
        BOOST_CHECK(LowerBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{12,5})));

        BOOST_CHECK_EQUAL(LowerBorder2.size(), 3);
        BOOST_CHECK(LowerBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{11,8})));
        BOOST_CHECK(LowerBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{5,9})));
        BOOST_CHECK(LowerBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{4,10})));

        BOOST_CHECK_EQUAL(LowerBorder3.size(), 5);
        BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{16,6})));
        BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{13,7})));
        BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{9,9})));
        BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{6,13})));
        BOOST_CHECK(LowerBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{3,14})));

        // upper borders
        BOOST_CHECK_EQUAL(UpperBorder3.size(), 6);
        BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{16,6})));
        BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{13,7})));
        BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{12,11})));
        BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{9,12})));
        BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{6,15})));
        BOOST_CHECK(UpperBorder3.contains(std::make_shared<FeatureVector>(std::vector<double>{5,17})));

        BOOST_CHECK_EQUAL(UpperBorder2.size(), 3);
        BOOST_CHECK(UpperBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{11,8})));
        BOOST_CHECK(UpperBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{8,11})));
        BOOST_CHECK(UpperBorder2.contains(std::make_shared<FeatureVector>(std::vector<double>{7,12})));

        BOOST_CHECK_EQUAL(UpperBorder1.size(), 2);
        BOOST_CHECK(UpperBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{17,5})));
        BOOST_CHECK(UpperBorder1.contains(std::make_shared<FeatureVector>(std::vector<double>{14,6})));

        BOOST_CHECK_EQUAL(UpperBorder0.size(), 5);
        BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{14,1})));
        BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{12,3})));
        BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{11,7})));
        BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{3,12})));
        BOOST_CHECK(UpperBorder0.contains(std::make_shared<FeatureVector>(std::vector<double>{2,15})));
}

BOOST_AUTO_TEST_CASE( containing_border ) {
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

    BOOST_CHECK( pLayer1->get_neg_pos_counts() == (std::pair<Sample::WeightType,Sample::WeightType>({50,10})) );
    BOOST_CHECK( pLayer2->get_neg_pos_counts() == (std::pair<Sample::WeightType,Sample::WeightType>({16,8})) );
    BOOST_CHECK( pLayer3->get_neg_pos_counts() == (std::pair<Sample::WeightType,Sample::WeightType>({9,18})) );
    BOOST_CHECK( pLayer4->get_neg_pos_counts() == (std::pair<Sample::WeightType,Sample::WeightType>({9,45})) );

    BOOST_TEST_MESSAGE("Creating border system");
    std::shared_ptr<BorderSystemCreator> pBSC = std::make_shared<BorderSystemCreator>();
    std::shared_ptr<BorderSystem> pBS = pBSC->from_layer_partitioning(pLD);
    BOOST_CHECK_EQUAL(pBS.use_count(), 1);
    std::vector<double> p;  std::pair<int, int> borders; std::pair<double, double> conf;
    p = {10,2}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {10,2}; conf = {1.0/6.0,1.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), conf.first);
    p = {1,12}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {16,1}; borders={1,1};BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {16,1}; conf = {1.0/3.0,1.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), conf.first);
    p = {7,10}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {7,10}; conf = {2.0/3.0,2.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), conf.first);
    p = {11,10}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {11,10}; conf = {5.0/6.0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), conf.first);
    p = {4,15}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);

    p = {0,11}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {2,15}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {2,7}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {3,12}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {5,4}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {8,3}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {9,1}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {12,3}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {11,7}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {14,1}; borders={0,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);

    p = {15,0}; borders={1,1}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {17,2}; borders={1,1}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {13,2}; borders={1,1}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {12,5}; borders={1,1}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {14,6}; borders={1,1}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {17,5}; borders={1,1}; BOOST_CHECK(pBS->containing_borders(p) == borders);

    p = {11,8}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {8,9}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {8,11}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {7,12}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {7,9}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {5,9}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {4,10}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {4,12}; borders={2,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);

    p = {16,6}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {13,7}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {12,9}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {12,11}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {9,9}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {9,11}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {9,12}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {6,13}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {6,15}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {3,14}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {3,17}; borders={3,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);

    p = {4,6}; borders={-1,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {4,6}; conf = {0,1.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), conf.second);
    p = {7,3}; borders={-1,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {1,10}; borders={-1,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {7.9,3.9}; borders={-1,0}; BOOST_CHECK(pBS->containing_borders(p) == borders);

    p = {7,13}; borders={3,4}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {7,13}; conf = {5.0/6.0,1}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), conf.first);
    p = {6.1,13.1}; borders={3,4}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {10,15}; borders={3,4}; BOOST_CHECK(pBS->containing_borders(p) == borders);

    p = {14.5,0.5}; borders={-1,1}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {14.5,0.5}; conf = {0,1.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (10+8)/(double)(50+10+16+8));
    p = {-1,16}; borders={-1,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {-1,16}; conf = {0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {-1,18}; borders={-1,4}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {-1,18}; conf = {0,1}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {18,-1}; borders={-1,4}; BOOST_CHECK(pBS->containing_borders(p) == borders);

    p = {12.5,1.5}; borders={0,1}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {12.5,1.5}; conf = {1.0/6.0,1.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (10+8)/(double)(50+10+16+8));
    p = {4,8}; borders={0,2}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {4,8}; conf = {1.0/6.0,2.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (10+8+18)/(double)(50+10+16+8+9+18));
    p = {3,13}; borders={0,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {3,13}; conf = {1.0/6.0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {1,18}; borders={0,4}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {1,18};  conf = {1.0/6.0,1}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));

    p = {15,5.5}; borders={1,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {15,5.5}; conf = {1.0/3.0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (8+18+45)/(double)(16+8+9+18+9+45));
    p = {18,1}; borders={1,4}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {18,1}; conf = {1.0/3.0,1}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (8+18+45)/(double)(16+8+9+18+9+45));

    p = {8,12}; borders={2,3}; BOOST_CHECK(pBS->containing_borders(p) == borders);
    p = {8,12}; conf = {2.0/3.0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pBS->confidence(p), (18+45)/(double)(9+18+9+45));
}

BOOST_AUTO_TEST_SUITE_END()