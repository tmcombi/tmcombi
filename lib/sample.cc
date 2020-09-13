#define BOOST_TEST_MODULE lib_test_sample
#include <boost/test/included/unit_test.hpp>

#include "sample.h"

BOOST_AUTO_TEST_CASE( basic_checks )
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
    Sample smpl(fn);
    BOOST_CHECK_EQUAL(  smpl.get_dim(), fn.get_dim() );
}
