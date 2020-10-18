#define BOOST_TEST_MODULE lib_test_feature_vector
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "feature_vector.h"

BOOST_AUTO_TEST_CASE( feature_vector_basics )
{
    FeatureVector fv({11, 22});
    fv.inc_weight_negatives(2).inc_weight_positives(3).dim();
    BOOST_TEST_MESSAGE("Testing feature vector: " << fv);
    BOOST_REQUIRE( fv.dim() == 2 );               // throws on error
    BOOST_CHECK( fv.dim() > 0 );        // continues on error
    BOOST_CHECK_EQUAL(  fv.get_weight_negatives(), 2 ); // continues on error
    BOOST_CHECK_EQUAL( fv.get_weight_positives(),  3 );
    BOOST_CHECK_EQUAL( fv.inc_weight_positives(4).get_weight_positives(), 7 );
    BOOST_REQUIRE_EQUAL( fv[0],  11 );
    BOOST_CHECK_EQUAL( fv[1],  22 );
}

BOOST_AUTO_TEST_CASE( feature_vector_from_string_buffer )
{
    FeatureVector
    fv("11,22,33,44,55,66,77",{3,2},5,"foo","66");
    BOOST_TEST_MESSAGE("Testing feature vector: " << fv);
    BOOST_CHECK_EQUAL(  fv.dim(), 2 );
    BOOST_CHECK_EQUAL(  fv[0], 44 );
    BOOST_CHECK_EQUAL(  fv[1], 33 );
    BOOST_CHECK(  fv.get_data() == std::vector<double>({44,33}) );
    BOOST_CHECK_EQUAL(  fv.get_weight_positives(), 1 );
    BOOST_CHECK_EQUAL(  fv.get_weight_negatives(), 0 );

    FeatureVector
            fv1("11,22,33,44,55,66,77",{3,2},5,"foo","66", 6);
    BOOST_TEST_MESSAGE("Testing feature vector: " << fv1);
    BOOST_CHECK_EQUAL(  fv1.get_weight_positives(), 77 );

    FeatureVector
            fv2("11,22,33,44,55,foo,77",{3,2},5,"foo","66", 6);
    BOOST_TEST_MESSAGE("Testing feature vector: " << fv2);
    BOOST_CHECK_EQUAL(  fv2.get_weight_negatives(), 77 );
 }

bool is_critical(const std::exception& ex ) { return true; }

BOOST_AUTO_TEST_CASE( feature_vector_exceptions ) {
    BOOST_TEST_MESSAGE("Testing exception operator[] out of range");
    FeatureVector fv("11,22,33,44,55,66,77",{3,2},5,"foo","66");
    BOOST_CHECK_EXCEPTION( fv[234], std::out_of_range, is_critical);

    BOOST_TEST_MESSAGE("Testing exception target_feature_index out of range");
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,66,77",{3,2},555,"foo","bar"), std::out_of_range, is_critical);

    BOOST_TEST_MESSAGE("Testing exception feature_index out of range");
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,bar,77",{333,2},5,"foo","bar"), std::out_of_range, is_critical);

    BOOST_TEST_MESSAGE("Testing exception weight_index out of range");
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,66,77",{3,2},5,"foo","bar",1024), std::out_of_range, is_critical);

    BOOST_TEST_MESSAGE("Testing exception for unknown class");
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,66,77",{3,2},5,"foo","bar"), std::invalid_argument, is_critical);

    BOOST_TEST_MESSAGE("Testing exception for negative weight");
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,bar,-77",{3,2},5,"foo","bar",6), std::invalid_argument, is_critical);

    BOOST_TEST_MESSAGE("Testing exception for non-numeric weight");
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,33,44,55,bar,ab",{3,2},5,"foo","bar",6), std::invalid_argument, is_critical);

    BOOST_TEST_MESSAGE("Testing exception for non-numeric feature value");
    BOOST_CHECK_EXCEPTION(
            FeatureVector("11,22,ab,44,55,bar,77",{3,2},5,"foo","bar"), std::invalid_argument, is_critical);
}

BOOST_AUTO_TEST_CASE( feature_vector_equal ) {
    FeatureVector fv1("11,22,33,44,55,66,77",{3,2},5,"foo","66");
    FeatureVector fv2("0,11,22,33,44,foo,77",{4,3},5,"foo","66");
    FeatureVector fv3("0,11,22,33,44,foo,77",{4,3,2},5,"foo","66");
    FeatureVector fv4("0,11,22,33,44,foo,77",{4,2,3},5,"foo","66");
    BOOST_TEST_MESSAGE("Testing feature vectors\n"
                               << "fv1 = " << fv1 << '\n'
                               << "fv2 = " << fv2 << '\n'
                               << "fv3 = " << fv3 << '\n'
                               << "fv4 = " << fv4);
    BOOST_CHECK_EQUAL(fv1, fv2);
    BOOST_CHECK_NE(fv1, fv3);
    BOOST_CHECK_NE(fv3, fv4);
}

BOOST_AUTO_TEST_CASE( feature_vector_comparison ) {
    FeatureVector fv1("11,22,33,44,55,66,77",{0,1},5,"foo","66");
    FeatureVector fv2("12,22,33,44,55,66,77",{0,1},5,"foo","66");
    FeatureVector fv3("11,23,33,44,55,66,77",{0,1},5,"foo","66");
    FeatureVector fv4("12,23,33,44,55,66,77",{0,1},5,"foo","66");
    BOOST_TEST_MESSAGE("Testing feature vectors\n"
                               << "fv1 = " << fv1 << '\n'
                               << "fv2 = " << fv2 << '\n'
                               << "fv3 = " << fv3 << '\n'
                               << "fv4 = " << fv4);
    BOOST_CHECK_GE( fv1, fv1 );
    BOOST_CHECK_GE( fv2, fv1 );
    BOOST_CHECK_GE( fv3, fv1 );
    BOOST_CHECK_GE( fv4, fv2 );
    BOOST_CHECK_GE( fv4, fv3 );
    BOOST_CHECK_GE( fv4, fv1 );

    BOOST_CHECK_GT( fv2, fv1 );
    BOOST_CHECK_GT( fv3, fv1 );
    BOOST_CHECK_GT( fv4, fv2 );
    BOOST_CHECK_GT( fv4, fv3 );
    BOOST_CHECK_GT( fv4, fv1 );

    BOOST_CHECK_LE( fv1, fv1 );
    BOOST_CHECK_LE( fv1, fv2 );
    BOOST_CHECK_LE( fv1, fv3 );
    BOOST_CHECK_LE( fv2, fv4 );
    BOOST_CHECK_LE( fv3, fv4 );
    BOOST_CHECK_LE( fv1, fv4 );

    BOOST_CHECK_LT( fv1, fv2 );
    BOOST_CHECK_LT( fv1, fv3 );
    BOOST_CHECK_LT( fv2, fv4 );
    BOOST_CHECK_LT( fv3, fv4 );
    BOOST_CHECK_LT( fv1, fv4 );

    BOOST_CHECK(!(fv2 <= fv1));
    BOOST_CHECK(!(fv2 < fv1));
    BOOST_CHECK(!(fv1 >= fv2));
    BOOST_CHECK(!(fv1 > fv2));

    BOOST_CHECK(!(fv3 <= fv2));
    BOOST_CHECK(!(fv2 <= fv3));
    BOOST_CHECK(!(fv3 < fv2));
    BOOST_CHECK(!(fv2 < fv3));

    BOOST_CHECK(!(fv3 >= fv2));
    BOOST_CHECK(!(fv2 >= fv3));
    BOOST_CHECK(!(fv3 > fv2));
    BOOST_CHECK(!(fv2 > fv3));
}

BOOST_AUTO_TEST_CASE( feature_vector_ptree ) {
    FeatureVector fv("11,22,33,44,55,66,77",{4,3,2},5,"foo","66");
    BOOST_TEST_MESSAGE("Feature vector: " << fv);

    boost::property_tree::ptree pt;
    fv.dump_to_ptree(pt);

    BOOST_CHECK_EQUAL(pt.size(), 4);

    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    FeatureVector fv1(pt);
    BOOST_TEST_MESSAGE("Read from ptree: " << fv1);
    BOOST_CHECK_EQUAL(fv.dim(), fv1.dim());
    BOOST_CHECK_EQUAL(fv.get_weight_negatives(), fv1.get_weight_negatives());
    BOOST_CHECK_EQUAL(fv.get_weight_positives(), fv1.get_weight_positives());
    BOOST_CHECK(fv.get_data() == fv1.get_data());
}
