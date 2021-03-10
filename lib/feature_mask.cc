#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_feature_mask
#endif
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "feature_mask.h"

BOOST_AUTO_TEST_SUITE( feature_mask )

BOOST_AUTO_TEST_CASE( basics )
{
    FeatureMask fm1(4);fm1[0] = fm1[2] = fm1.sign(2) = true;
    boost::dynamic_bitset<> bs1(std::string("0101"));
    boost::dynamic_bitset<> bs2(std::string("0100"));
    FeatureMask fm2(bs1);fm2.sign(2) = true;
    FeatureMask fm3(bs1,bs2);
    FeatureMask fm4("0101");fm4.sign(2) = true;
    FeatureMask fm5("0101", "0100");

    BOOST_TEST_MESSAGE("Mask: " << fm1.to_strings().first);
    BOOST_TEST_MESSAGE("Sign: " << fm1.to_strings().second);
    BOOST_CHECK_EQUAL( fm1[0],  true );
    BOOST_CHECK_EQUAL( fm1[1],  false );
    BOOST_CHECK_EQUAL( fm1[2],  true );
    BOOST_CHECK_EQUAL( fm1[3],  false );
    BOOST_CHECK_EQUAL( fm1.sign(0),  false );
    BOOST_CHECK_EQUAL( fm1.sign(1),  false );
    BOOST_CHECK_EQUAL( fm1.sign(2),  true );
    BOOST_CHECK_EQUAL( fm1.sign(3),  false );
    BOOST_CHECK_EQUAL( fm2[0],  true );
    BOOST_CHECK_EQUAL( fm2[1],  false );
    BOOST_CHECK_EQUAL( fm2[2],  true );
    BOOST_CHECK_EQUAL( fm2[3],  false );
    BOOST_CHECK_EQUAL( fm2.sign(0),  false );
    BOOST_CHECK_EQUAL( fm2.sign(1),  false );
    BOOST_CHECK_EQUAL( fm2.sign(2),  true );
    BOOST_CHECK_EQUAL( fm2.sign(3),  false );
    BOOST_CHECK_EQUAL( fm3[0],  true );
    BOOST_CHECK_EQUAL( fm3[1],  false );
    BOOST_CHECK_EQUAL( fm3[2],  true );
    BOOST_CHECK_EQUAL( fm3[3],  false );
    BOOST_CHECK_EQUAL( fm3.sign(0),  false );
    BOOST_CHECK_EQUAL( fm3.sign(1),  false );
    BOOST_CHECK_EQUAL( fm3.sign(2),  true );
    BOOST_CHECK_EQUAL( fm3.sign(3),  false );
    BOOST_CHECK_EQUAL( fm4[0],  true );
    BOOST_CHECK_EQUAL( fm4[1],  false );
    BOOST_CHECK_EQUAL( fm4[2],  true );
    BOOST_CHECK_EQUAL( fm4[3],  false );
    BOOST_CHECK_EQUAL( fm4.sign(0),  false );
    BOOST_CHECK_EQUAL( fm4.sign(1),  false );
    BOOST_CHECK_EQUAL( fm4.sign(2),  true );
    BOOST_CHECK_EQUAL( fm4.sign(3),  false );
    BOOST_CHECK_EQUAL( fm5[0],  true );
    BOOST_CHECK_EQUAL( fm5[1],  false );
    BOOST_CHECK_EQUAL( fm5[2],  true );
    BOOST_CHECK_EQUAL( fm5[3],  false );
    BOOST_CHECK_EQUAL( fm5.sign(0),  false );
    BOOST_CHECK_EQUAL( fm5.sign(1),  false );
    BOOST_CHECK_EQUAL( fm5.sign(2),  true );
    BOOST_CHECK_EQUAL( fm5.sign(3),  false );
}

BOOST_AUTO_TEST_CASE( ptree ) {
    FeatureMask fm("1101","0100");

    boost::property_tree::ptree pt;
    fm.dump_to_ptree(pt);

    BOOST_CHECK_EQUAL(pt.size(), 3);

    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    FeatureMask fm1(pt);
    BOOST_CHECK_EQUAL(fm.dim(), fm1.dim());
    for (size_t i = 0; i < fm.dim(); i++) {
        BOOST_CHECK_EQUAL( fm[i],  fm1[i] );
        BOOST_CHECK_EQUAL( fm.sign(i),  fm1.sign(i) );
    }
}

BOOST_AUTO_TEST_SUITE_END()