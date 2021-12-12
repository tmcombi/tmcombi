#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_layer_partitioning
#endif
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"
#include "layer_partitioning.h"

BOOST_AUTO_TEST_SUITE( layer_partitioning )

BOOST_AUTO_TEST_CASE( basics ) {
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

    std::string data_buffer1("1,8,34,44,v2,1,77\n"
                             "2,2,34,44,v1,2,77\n"
                             "2,5,34,44,v2,3,77\n"
                             "3,7,34,44,v2,1,77\n"
                             "3,9,34,44,v1,2,77\n"
                             "4,1,34,44,v2,3,77\n"
                             "4,5,34,44,v2,1,77\n"
                             "5,8,34,44,v1,2,77\n"
                             "6,0,34,44,v2,3,77\n"
                             "7,1,34,44,v2,3,77\n"
                             "7,5,34,44,v2,3,77\n"
                             "8,2,34,44,v2,3,77\n"
    );

    std::stringstream ss_buffer1((std::stringstream(data_buffer1)));

    SampleCreator sample_creator1;
    sample_creator1.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample1 = sample_creator1.from_stream(ss_buffer1);
    BOOST_TEST_MESSAGE("Create a layer partitioning based on Sample1: " << *pSample1);
    std::shared_ptr<LayerPartitioning> pLD = std::make_shared<LayerPartitioning>();
    pLD->push_back(pSample1);

    BOOST_TEST_MESSAGE("Splitting the only layer within the layer partitioning");
    boost::dynamic_bitset<> db1(pSample1->size());
    db1[3] = db1[4] = db1[7] = true;
    auto it = pLD->begin();
    pLD->split_layer(it, db1);

    BOOST_TEST_MESSAGE("Lower layer: " << *pLD->begin());
    BOOST_TEST_MESSAGE("Upper layer: " << *it);

    BOOST_CHECK_EQUAL(pLD->size(), 2);
    BOOST_CHECK_EQUAL(pLD->dim(), pSample1->dim());
    BOOST_CHECK_EQUAL((*pLD->begin())->dim(), pSample1->dim());
    BOOST_CHECK_EQUAL((*it)->dim(), pSample1->dim());

    BOOST_CHECK_EQUAL((*it)->size(),3);
    BOOST_CHECK_EQUAL((*it)->size() + (*pLD->begin())->size() , pSample1->size());
    BOOST_CHECK_EQUAL((*it)->get_neg_pos_counts().first + (*pLD->begin())->get_neg_pos_counts().first , pSample1->get_neg_pos_counts().first );
    BOOST_CHECK_EQUAL((*it)->get_neg_pos_counts().second  + (*pLD->begin())->get_neg_pos_counts().second, pSample1->get_neg_pos_counts().second);

    BOOST_CHECK((*pLD->begin())->contains((*pSample1)[0]));
    BOOST_CHECK((*pLD->begin())->contains((*pSample1)[1]));
    BOOST_CHECK((*pLD->begin())->contains((*pSample1)[5]));
    BOOST_CHECK((*pLD->begin())->contains((*pSample1)[6]));

    BOOST_CHECK((*it)->contains((*pSample1)[4]));
    BOOST_CHECK((*it)->contains((*pSample1)[7]));
    BOOST_CHECK((*it)->contains((*pSample1)[3]));

    BOOST_CHECK_GE(*(*it), *(*pLD->begin()));
    BOOST_CHECK_LE(*(*pLD->begin()), *(*it));
    BOOST_CHECK(!(*(*it) <= *(*pLD->begin())));
    BOOST_CHECK(!(*(*pLD->begin()) >= *(*it)));
    BOOST_CHECK((*pLD->begin())->has_no_intersection_with(*(*it)));
    BOOST_CHECK((*it)->has_no_intersection_with(*(*pLD->begin())));

    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    BOOST_CHECK_EQUAL(pSample1.use_count(), 1);

    BOOST_TEST_MESSAGE("Splitting the lowest layer once again");
    boost::dynamic_bitset<> db2((*pLD->begin())->size());
    db2[0] = db2[7] = db2[4] = true;
    auto middle = pLD->begin();
    pLD->split_layer(middle, db2);
    BOOST_CHECK_EQUAL(pLD->size(), 3);
    BOOST_TEST_MESSAGE("First layer: " << *(*pLD->begin()));
    BOOST_TEST_MESSAGE("Second layer: " << *(*middle));
    BOOST_TEST_MESSAGE("Third layer: " << *(*it));
    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    BOOST_CHECK_EQUAL((*it)->size() + (*middle)->size() + (*pLD->begin())->size() , pSample1->size());
}

BOOST_AUTO_TEST_CASE( ptree ) {
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

    std::string data_buffer1("1,8,34,44,v2,1,77\n"
                             "2,2,34,44,v1,2,77\n"
                             "2,5,34,44,v2,3,77\n"
                             "3,7,34,44,v2,1,77\n"
                             "3,9,34,44,v1,2,77\n"
                             "4,1,34,44,v2,3,77\n"
                             "4,5,34,44,v2,1,77\n"
                             "5,8,34,44,v1,2,77\n"
                             "6,0,34,44,v2,3,77\n"
                             "7,1,34,44,v2,3,77\n"
                             "7,5,34,44,v2,3,77\n"
                             "8,2,34,44,v2,3,77\n"
    );

    std::stringstream ss_buffer1((std::stringstream(data_buffer1)));
    SampleCreator sample_creator1;
    sample_creator1.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample1 = sample_creator1.from_stream(ss_buffer1);
    BOOST_TEST_MESSAGE("Create a layer partitioning based on Sample1: " << *pSample1);
    std::shared_ptr<LayerPartitioning> pLD = std::make_shared<LayerPartitioning>();
    pLD->push_back(pSample1);
    BOOST_TEST_MESSAGE("Splitting the only layer within the layer partitioning");
    boost::dynamic_bitset<> db1(pSample1->size());
    db1[3] = db1[4] = db1[7] = true;
    auto it = pLD->begin();
    pLD->split_layer(it, db1);
    BOOST_TEST_MESSAGE("Splitting the lowest layer once again");
    boost::dynamic_bitset<> db2((*pLD->begin())->size());
    db2[0] = db2[7] = db2[4] = true;
    auto middle = pLD->begin();
    pLD->split_layer(middle, db2);
    BOOST_TEST_MESSAGE("First layer: " << *(*pLD->begin()));
    BOOST_TEST_MESSAGE("Second layer: " << *(*middle));
    BOOST_TEST_MESSAGE("Third layer: " << *(*it));

    boost::property_tree::ptree pt;
    pLD->dump_to_ptree(pt);

    BOOST_CHECK_EQUAL(pt.size(), 3);

    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Layer partitioning as json:\n" << ss.str());


    std::shared_ptr<LayerPartitioning> pLD1 = std::make_shared<LayerPartitioning>(pt);
    BOOST_TEST_MESSAGE("Read layer partitioning from the ptree");
    BOOST_TEST_MESSAGE("First layer: " << *(*pLD1->begin()));
    BOOST_TEST_MESSAGE("Second layer: " << *(*(pLD1->begin() + 1)));
    BOOST_TEST_MESSAGE("Third layer: " << *(*(pLD1->begin() + 2)));
    BOOST_CHECK_EQUAL(pLD1->consistent(),true);
}

BOOST_AUTO_TEST_CASE( thirtysix_points ) {
    BOOST_TEST_MESSAGE("More comprehensive example");
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

    boost::dynamic_bitset<> db4(36);
    boost::dynamic_bitset<> db3(27);
    boost::dynamic_bitset<> db2(19);
    db4[27] = db4[28] = db4[29] = db4[30] = db4[31] = db4[32] = db4[33] = db4[34] = db4[35] = true;
    db3[19] = db3[20] = db3[21] = db3[22] = db3[23] = db3[24] = db3[25] = db3[26] = true;
    db2[12] = db2[13] = db2[14] = db2[15] = db2[16] = db2[17] = db2[18] = true;

    std::shared_ptr<LayerPartitioning> pLD = std::make_shared<LayerPartitioning>();
    pLD->push_back(pSample);
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pSample)), pSample->size());
    size_t num_edges = boost::num_edges(*pLD->get_graph(pSample));

    BOOST_TEST_MESSAGE("Splitting the lowest layer three times into 4 layers");
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

    BOOST_CHECK_LE(*pLayer1, *pLayer2);
    BOOST_CHECK_LE(*pLayer1, *pLayer3);
    BOOST_CHECK_LE(*pLayer1, *pLayer4);
    BOOST_CHECK_LE(*pLayer2, *pLayer3);
    BOOST_CHECK_LE(*pLayer2, *pLayer4);
    BOOST_CHECK_LE(*pLayer3, *pLayer4);
    BOOST_CHECK_GE(*pLayer4, *pLayer3);
    BOOST_CHECK_GE(*pLayer3, *pLayer2);
    BOOST_CHECK_GE(*pLayer2, *pLayer1);

    BOOST_CHECK_EQUAL((double)pLayer1->get_neg_pos_counts().second / (double)pLayer1->get_neg_pos_counts().first, (double)1/(double)5 );
    BOOST_CHECK_EQUAL((double)pLayer2->get_neg_pos_counts().second / (double)pLayer2->get_neg_pos_counts().first, (double)1/(double)2 );
    BOOST_CHECK_EQUAL((double)pLayer3->get_neg_pos_counts().second / (double)pLayer3->get_neg_pos_counts().first, (double)2/(double)1 );
    BOOST_CHECK_EQUAL((double)pLayer4->get_neg_pos_counts().second / (double)pLayer4->get_neg_pos_counts().first, (double)5/(double)1 );

    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer1)), pLayer1->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer2)), pLayer2->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer3)), pLayer3->size());
    BOOST_CHECK_EQUAL(boost::num_vertices(*pLD->get_graph(pLayer4)), pLayer4->size());

    pLD->merge_two_layers(pLD->begin()+1);
    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    pLD->merge_two_layers(pLD->begin()+1);
    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    pLD->merge_two_layers(pLD->begin());
    BOOST_CHECK_EQUAL(pLD->consistent(),true);

    BOOST_CHECK_EQUAL(num_edges, boost::num_edges(*pLD->get_graph(*pLD->begin())));
}

BOOST_AUTO_TEST_SUITE_END()