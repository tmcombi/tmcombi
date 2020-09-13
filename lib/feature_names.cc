#define BOOST_TEST_MODULE lib_test_feature_names
#include <boost/test/included/unit_test.hpp>

#include "feature_names.h"

BOOST_AUTO_TEST_CASE( check_input_string_stream )
{
    std::string buffer("| this is comment\n"
                       "target_feature.| one more comment\n"
                       "\n"
                       "   feature1: continuous.   \n"
                       "feature2: continuous.   | third comment\n"
                       "feature3: ignore.\n"
                       "feature4: continuous.\n"
                       "target_feature: v1, v2.\n"
                       "case weight: continuous.\n"
                       "feature5: continuous.\n"
                       "\n"
                       "  | one more comment here\n"
                       "\n");
    BOOST_TEST_MESSAGE("Testing names created from buffer:\n"
    << "#######################################################\n"
    << buffer
    << "#######################################################\n" );
    FeatureNames fn((std::stringstream(buffer)));
    BOOST_CHECK_EQUAL(fn.get_dim(), 4);
    BOOST_CHECK(fn.get_feature_indices() == std::vector<unsigned int>({0,1,3,6}));
    BOOST_CHECK(fn.get_feature_names() == std::vector<std::string>({
                          "feature1",
                          "feature2",
                          "feature4",
                          "feature5"
                      }));
    BOOST_CHECK_EQUAL(fn.get_target_feature_index(), 4);
    BOOST_CHECK_EQUAL(fn.get_target_feature_name(), "target_feature");
    BOOST_CHECK_EQUAL(fn.get_negatives_label(), "v1");
    BOOST_CHECK_EQUAL(fn.get_positives_label(), "v2");
    BOOST_CHECK_EQUAL(fn.get_weight_index(), 5);
}

bool is_critical(const std::exception& ex ) { return true; }

BOOST_AUTO_TEST_CASE( check_exceptions ) {
    //BOOST_TEST_MESSAGE("Testing exception operator[] out of range");
    //FeatureVector fv("11,22,33,44,55,66,77",{3,2},5,"foo","66");
    //BOOST_CHECK_EXCEPTION( fv[234], std::out_of_range, is_critical);
}
