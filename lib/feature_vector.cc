#define BOOST_TEST_MODULE lib_test_feature_vector
#include <boost/test/included/unit_test.hpp>
#include <boost/lambda/lambda.hpp>

#include "feature_vector.h"

BOOST_AUTO_TEST_CASE( basic_checks )
{
    FeatureVector fv({11, 22});
    fv.inc_weight_negatives(2).inc_weight_positives(3).get_dim();
    BOOST_TEST_MESSAGE("Testing feature vector: " << fv);
    BOOST_REQUIRE( fv.get_dim() == 2 );               // throws on error
    BOOST_CHECK( fv.get_dim() > 0 );        // continues on error
    BOOST_CHECK_EQUAL(  fv.get_weight_negatives(), 2 ); // continues on error
    BOOST_CHECK_EQUAL( fv.get_weight_positives(),  3 );
    BOOST_CHECK_EQUAL( fv.inc_weight_positives(4).get_weight_positives(), 7 );
    BOOST_REQUIRE_EQUAL( fv[0],  11 );
    BOOST_CHECK_EQUAL( fv[1],  22 );
}

bool is_critical(const std::exception& ex ) { return true; }

BOOST_AUTO_TEST_CASE( check_string_buffer_input )
{
    FeatureVector
    fv("11,22,33,44,55,66,77",{3,2},5,"foo","66");
    BOOST_TEST_MESSAGE("Testing feature vector: " << fv);
    BOOST_CHECK_EQUAL(  fv.get_dim(), 2 );
    BOOST_CHECK_EQUAL(  fv[0], 44 );
    BOOST_CHECK_EQUAL(  fv[1], 33 );
    BOOST_CHECK_EQUAL(  fv.get_weight_positives(), 1 );
    BOOST_CHECK_EQUAL(  fv.get_weight_negatives(), 0 );

    FeatureVector
            fv1("11,22,33,44,55,66,77",{3,2},5,"foo","66", 6);
    BOOST_CHECK_EQUAL(  fv1.get_weight_positives(), 77 );

    FeatureVector
            fv2("11,22,33,44,55,foo,77",{3,2},5,"foo","66", 6);
    BOOST_CHECK_EQUAL(  fv2.get_weight_negatives(), 77 );
 }

BOOST_AUTO_TEST_CASE( check_string_buffer_input_exceptions ) {
    //operator[] out of range
    FeatureVector fv("11,22,33,44,55,66,77",{3,2},5,"foo","66");
    BOOST_CHECK_EXCEPTION( fv[234], std::out_of_range, is_critical);

    //target_feature_index out of range
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,66,77",{3,2},555,"foo","bar"), std::out_of_range, is_critical);
    //feature_index out of range
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,bar,77",{333,2},5,"foo","bar"), std::out_of_range, is_critical);
    //weight_index out of range
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,66,77",{3,2},5,"foo","bar",1024), std::out_of_range, is_critical);

    //class not known
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,66,77",{3,2},5,"foo","bar"), std::invalid_argument, is_critical);
    //weight negative
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,bar,-77",{3,2},5,"foo","bar",6), std::invalid_argument, is_critical);
    //weight not numeric
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,bar,ab",{3,2},5,"foo","bar",6), std::invalid_argument, is_critical);
    //feature not numeric
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,ab,44,55,bar,77",{3,2},5,"foo","bar"), std::invalid_argument, is_critical);
}

    /*
   BOOST_CHECK_MESSAGE( add( 2,2 ) == 4,  // #6 continues on error
                        "add(..) result: " << add( 2,2 ) );
   */