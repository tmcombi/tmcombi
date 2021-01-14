#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_feature_names
#endif
#include <boost/test/included/unit_test.hpp>

#include "feature_names.h"

#ifndef TMC_UNIT_TESTS
bool is_critical(const std::exception& ) { return true; }
#endif

BOOST_AUTO_TEST_SUITE( feature_names )

BOOST_AUTO_TEST_CASE( from_string_stream )
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
    std::stringstream ss((std::stringstream(buffer)));
    FeatureNames fn(ss);
    BOOST_CHECK_EQUAL(fn.dim(), 4);
    BOOST_CHECK(fn.get_feature_indices() == std::vector<size_t>({0,1,3,6}));
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

BOOST_AUTO_TEST_CASE( exceptions ) {
    std::string buffer("target_feature.\n"
                       "feature1: continuous.\n"
                       "feature2: continuous.\n"
                       "target_feature: v1, v2, v3.\n");
    BOOST_TEST_MESSAGE("Testing names created from buffer:\n"
                               << "#######################################################\n"
                               << buffer
                               << "#######################################################\n" );
    std::stringstream ss((std::stringstream(buffer)));
    BOOST_CHECK_EXCEPTION( FeatureNames fn(ss), std::invalid_argument, is_critical);
}

BOOST_AUTO_TEST_CASE( from_file ) {
    const std::string names_file("data/tmc_paper_9/tmc_paper.names");

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);
    BOOST_CHECK_EQUAL(pFN->dim(), 2);
}

BOOST_AUTO_TEST_SUITE_END()
