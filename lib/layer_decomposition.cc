#define BOOST_TEST_MODULE lib_test_layer_decomposition
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"
#include "layer_decomposition.h"

BOOST_AUTO_TEST_CASE( layer_decomposition_basics ) {
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
    BOOST_TEST_MESSAGE("Create a layer decomposition based on Sample1: " << *pSample1);
    std::shared_ptr<LayerDecomposition> pLD = std::make_shared<LayerDecomposition>(pSample1);

    BOOST_TEST_MESSAGE("Splitting the only layer within the layer decomposition");
    boost::dynamic_bitset<> db1(pSample1->get_size());
    db1[3] = db1[4] = db1[7] = true;
    auto it = pLD->begin();
    it = pLD->split_layer(it, db1);

    BOOST_TEST_MESSAGE("Lower layer: " << *pLD->begin());
    BOOST_TEST_MESSAGE("Upper layer: " << *it);

    BOOST_CHECK_EQUAL(pLD->get_size(), 2);
    BOOST_CHECK_EQUAL(pLD->get_dim(), pSample1->get_dim());
    BOOST_CHECK_EQUAL((*pLD->begin())->get_dim(), pSample1->get_dim());
    BOOST_CHECK_EQUAL((*it)->get_dim(), pSample1->get_dim());

    BOOST_CHECK_EQUAL((*it)->get_size(),3);
    BOOST_CHECK_EQUAL((*it)->get_size() + (*pLD->begin())->get_size() , pSample1->get_size());
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
    boost::dynamic_bitset<> db2((*pLD->begin())->get_size());
    db2[0] = db2[7] = db2[4] = true;
    auto middle = pLD->split_layer(pLD->begin(), db2);
    BOOST_CHECK_EQUAL(pLD->get_size(), 3);
    BOOST_TEST_MESSAGE("First layer: " << *(*pLD->begin()));
    BOOST_TEST_MESSAGE("Second layer: " << *(*middle));
    BOOST_TEST_MESSAGE("Third layer: " << *(*it));
    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    BOOST_CHECK_EQUAL((*it)->get_size() + (*middle)->get_size() + (*pLD->begin())->get_size() , pSample1->get_size());
}
