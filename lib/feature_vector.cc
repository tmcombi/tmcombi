#include <ctime>
#define BOOST_TEST_MODULE lib_test_feature_vector
#include <boost/test/included/unit_test.hpp>
#include <boost/lambda/lambda.hpp>

#include "feature_vector.h"

BOOST_AUTO_TEST_CASE( basic_checks )
{
    FeatureVector fv({11, 22});
    unsigned int dim = fv.inc_weight_negatives(2).inc_weight_positives(3).get_dim();

    BOOST_CHECK( fv.get_dim() == 2 );        // #1 continues on error

    BOOST_REQUIRE( dim == 2 );      // #2 throws on error

    if( fv.get_weight_negatives() != 2 )
        BOOST_ERROR( "Ouch..." );            // #3 continues on error

    if( fv.get_weight_positives() != 3 )
        BOOST_FAIL( "Ouch..." );             // #4 throws on error

    BOOST_CHECK_EQUAL( fv.inc_weight_positives(4).get_weight_positives(), 7 );	  // #7 continues on error
}

BOOST_AUTO_TEST_CASE( test_data )
{
    BOOST_CHECK_MESSAGE( 4 == 4,  // #6 continues on error
                         "add(..) result: " << 4 );
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