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

bool is_critical( int code ) { return code < 0; }

BOOST_AUTO_TEST_CASE( check_string_buffer_input )
{
    FeatureVector fv("11,22,33,44,55,66,77",{3,2},6,"55","6g");
    BOOST_TEST_MESSAGE("Testing feature vector: " << fv);
    BOOST_CHECK_EQUAL(  fv.get_dim(), 2 );
    BOOST_CHECK_EQUAL(  fv[0], 33 );
    BOOST_CHECK_EQUAL(  fv[1], 22 );
    BOOST_CHECK_EQUAL(  fv.get_weight_positives(), 1 );
    BOOST_CHECK_EQUAL(  fv.get_weight_negatives(), 0 );

    BOOST_TEST_MESSAGE("Non existing element: " << fv[234]);
    BOOST_CHECK_EXCEPTION( fv[234], int, is_critical);
    BOOST_CHECK_EXCEPTION( throw(-234), int, is_critical);
 }
 /*
// seven ways to detect and report the same error:
BOOST_CHECK( add( 2,2 ) == 4 );        // #1 continues on error

BOOST_REQUIRE( add( 2,2 ) == 4 );      // #2 throws on error

if( add( 2,2 ) != 4 )
BOOST_ERROR( "Ouch..." );            // #3 continues on error

if( add( 2,2 ) != 4 )
BOOST_FAIL( "Ouch..." );             // #4 throws on error

if( add( 2,2 ) != 4 ) throw "Ouch..."; // #5 throws on error

BOOST_CHECK_MESSAGE( add( 2,2 ) == 4,  // #6 continues on error
                     "add(..) result: " << add( 2,2 ) );

BOOST_CHECK_EQUAL( add( 2,2 ), 4 );	  // #7 continues on error
*/