#define BOOST_TEST_MODULE lib_test_feature_names
#include <boost/test/included/unit_test.hpp>

#include "feature_names.h"

BOOST_AUTO_TEST_CASE( basic_checks )
{
    //FeatureVector fv({11, 22});
    //fv.inc_weight_negatives(2).inc_weight_positives(3).get_dim();
    //BOOST_TEST_MESSAGE("Testing feature vector: " << fv);
}

bool is_critical(const std::exception& ex ) { return true; }

BOOST_AUTO_TEST_CASE( check_exceptions ) {
    //BOOST_TEST_MESSAGE("Testing exception operator[] out of range");
    //FeatureVector fv("11,22,33,44,55,66,77",{3,2},5,"foo","66");
    //BOOST_CHECK_EXCEPTION( fv[234], std::out_of_range, is_critical);
}
