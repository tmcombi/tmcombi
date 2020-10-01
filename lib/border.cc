#define BOOST_TEST_MODULE lib_test_border
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>
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

    boost::property_tree::ptree pt;
    pLower->dump_to_ptree(pt);

    BOOST_CHECK_EQUAL(pt.size(), 5);

    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    const std::shared_ptr<Sample> pLower1 = std::make_shared<Border>(pt);
    BOOST_TEST_MESSAGE("Read from ptree: " << *pLower1);
    BOOST_CHECK_EQUAL(pLower->get_dim(), pLower1->get_dim());
    BOOST_CHECK_EQUAL(pLower->get_size(), pLower1->get_size());
    BOOST_CHECK(pLower->get_neg_pos_counts() == pLower1->get_neg_pos_counts());
    for (unsigned int i = 0; i < pLower->get_size(); ++i)
        BOOST_CHECK((*pLower)[i]->get_data() == (*pLower1)[i]->get_data());
}

BOOST_AUTO_TEST_CASE( border_point_check_2D ) {
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
    const std::shared_ptr<Border> pLower = sample_creator2.lower_border(pSample1);
    const std::shared_ptr<Border> pUpper = sample_creator2.upper_border(pSample1);
    BOOST_TEST_MESSAGE("Lower border: " << *pLower);
    BOOST_TEST_MESSAGE("Upper border: " << *pUpper);

    BOOST_CHECK(!pLower->point_above(std::vector<double>({0,0})));
    BOOST_CHECK(!pLower->point_above(std::vector<double>({3,1.5})));
    BOOST_CHECK(!pLower->point_above(std::vector<double>({0,10})));
    BOOST_CHECK(pLower->point_above(std::vector<double>({1,8})));
    BOOST_CHECK(pLower->point_above(std::vector<double>({2,8})));
    BOOST_CHECK(pLower->point_above(std::vector<double>({5,5})));
    BOOST_CHECK(pLower->point_above(std::vector<double>({5,2})));

    BOOST_CHECK(!pUpper->point_below(std::vector<double>({10,10})));
    BOOST_CHECK(!pUpper->point_below(std::vector<double>({6,6})));
    BOOST_CHECK(!pUpper->point_below(std::vector<double>({4,8.5})));
    BOOST_CHECK(!pUpper->point_below(std::vector<double>({9,1})));
    BOOST_CHECK(pUpper->point_below(std::vector<double>({0,0})));
    BOOST_CHECK(pUpper->point_below(std::vector<double>({3,9})));
    BOOST_CHECK(pUpper->point_below(std::vector<double>({3,8})));
    BOOST_CHECK(pUpper->point_below(std::vector<double>({4,3})));

    BOOST_CHECK_EQUAL(pLower->consistent(), true);
    BOOST_CHECK_EQUAL(pUpper->consistent(), true);

    for (double x = 0; x <=10; x+=0.5)
       for  (double y = 0; y <=10; y+=0.5) {
           if (pUpper->point_above(std::vector<double>({x, y}), false) !=
                   pUpper->point_above(std::vector<double>({x, y}), true))
               BOOST_TEST_FAIL("Fast and slow imps. for point_above produces different results for"
               << " x=" << x << ", y=" << y);
           if (pLower->point_below(std::vector<double>({x, y}), false) !=
                   pLower->point_below(std::vector<double>({x, y}), true))
               BOOST_TEST_FAIL("Fast and slow imps. for point_below produces different results for"
                                       << " x=" << x << ", y=" << y);
       }
}

BOOST_AUTO_TEST_CASE( border_point_check_multiD ) {
    std::string names_buffer("| this is comment\n"
                             "target_feature.| one more comment\n"
                             "\n"
                             "   feature1: continuous.   \n"
                             "feature2: continuous.   | third comment\n"
                             "feature3: continuous.\n"
                             "feature4: continuous.\n"
                             "target_feature: v1, v2.\n"
                             "case weight: continuous.\n"
                             "feature5: ignore.\n"
                             "\n"
                             "  | one more comment here\n"
                             "\n");
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);

    std::string data_buffer1("1,8,1,3,v2,1,77\n"
                             "2,2,2,4,v1,2,77\n"
                             "2,5,3,3,v2,3,77\n"
                             "3,7,4,2,v2,1,77\n"
                             "3,9,3,1,v1,2,77\n"
                             "4,1,2,2,v2,3,77\n"
                             "4,5,1,3,v2,1,77\n"
                             "5,8,2,4,v1,2,77\n"
                             "6,0,3,3,v2,3,77\n"
                             "7,1,4,2,v2,3,77\n"
                             "7,5,3,1,v2,3,77\n"
                             "8,2,2,2,v2,3,77\n"
    );

    std::stringstream ss_buffer1((std::stringstream(data_buffer1)));

    SampleCreator sample_creator1;
    sample_creator1.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample1 = sample_creator1.from_stream(ss_buffer1);
    BOOST_TEST_MESSAGE("Sample1: " << *pSample1);

    SampleCreator sample_creator2;
    const std::shared_ptr<Border> pLower = sample_creator2.lower_border(pSample1);
    const std::shared_ptr<Border> pUpper = sample_creator2.upper_border(pSample1);
    BOOST_TEST_MESSAGE("Lower border: " << *pLower);
    BOOST_TEST_MESSAGE("Upper border: " << *pUpper);

    BOOST_CHECK(!pLower->point_above(std::vector<double>({1,8,1,2})));
    BOOST_CHECK(pLower->point_above(std::vector<double>({1,8,1,3})));

    BOOST_CHECK(pUpper->point_below(std::vector<double>({2,5,3,3})));
    BOOST_CHECK(!pUpper->point_below(std::vector<double>({2,5,3,4})));

    BOOST_CHECK_EQUAL(pLower->consistent(), true);
    BOOST_CHECK_EQUAL(pUpper->consistent(), true);

    for (double x = 0; x <=10; x+=0.5)
        for  (double y = 0; y <=10; y+=0.5)
            for (double z = 0; z <=10; z+=0.5)
                for  (double t = 0; t <=10; t+=0.5) {
            if (pUpper->point_above(std::vector<double>({x,y,z,t}), false) !=
                pUpper->point_above(std::vector<double>({x,y,z,t}), true))
                BOOST_TEST_FAIL("Fast and slow imps. for point_above produces different results for"
                                        << " x=" << x << ", y=" << y << " z=" << z << ", t=" << t);
            if (pLower->point_below(std::vector<double>({x,y,z,t}), false) !=
                pLower->point_below(std::vector<double>({x,y,z,t}), true))
                BOOST_TEST_FAIL("Fast and slow imps. for point_below produces different results for"
                                        << " x=" << x << ", y=" << y << " z=" << z << ", t=" << t);
        }
}
