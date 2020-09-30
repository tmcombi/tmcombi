#define BOOST_TEST_MODULE lib_test_border
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"

BOOST_AUTO_TEST_CASE( border_basics ) {
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
    BOOST_TEST_MESSAGE("Sample1: " << *pSample1);

    SampleCreator sample_creator2;
    const std::shared_ptr<Sample> pLower = sample_creator2.lower_border(pSample1);
    const std::shared_ptr<Sample> pUpper = sample_creator2.upper_border(pSample1);
    BOOST_TEST_MESSAGE("Lower border: " << *pLower);
    BOOST_TEST_MESSAGE("Upper border: " << *pUpper);

    BOOST_CHECK(pSample1->get_dim() ==pLower->get_dim());
    BOOST_CHECK(pSample1->get_neg_pos_counts() ==pLower->get_neg_pos_counts());
    BOOST_CHECK(pSample1->get_neg_pos_counts() ==pUpper->get_neg_pos_counts());

    BOOST_CHECK_EQUAL(pLower->get_size(), 4);
    BOOST_CHECK_EQUAL(pUpper->get_size(), 4);

    BOOST_CHECK(pLower->contains((*pSample1)[0]));
    BOOST_CHECK(pLower->contains((*pSample1)[1]));
    BOOST_CHECK(pLower->contains((*pSample1)[5]));
    BOOST_CHECK(pLower->contains((*pSample1)[8]));

    BOOST_CHECK(pUpper->contains((*pSample1)[4]));
    BOOST_CHECK(pUpper->contains((*pSample1)[7]));
    BOOST_CHECK(pUpper->contains((*pSample1)[10]));
    BOOST_CHECK(pUpper->contains((*pSample1)[11]));

    BOOST_CHECK_GE(*pUpper, *pLower);
    BOOST_CHECK_LE(*pLower, *pUpper);
    BOOST_CHECK(!(*pUpper <= *pLower));
    BOOST_CHECK(!(*pLower >= *pUpper));
    BOOST_CHECK(pLower->has_no_intersection_with(*pUpper));
    BOOST_CHECK(pUpper->has_no_intersection_with(*pLower));
}

BOOST_AUTO_TEST_CASE( border_ptree ) {
    //todo: implement
}

BOOST_AUTO_TEST_CASE( border_fast ) {
    //todo: implement compare to slow
}
