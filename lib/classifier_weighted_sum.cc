#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_train_classifier_weighted_sum
#endif
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "classifier_weighted_sum.h"

BOOST_AUTO_TEST_SUITE( classifier_weighted_sum )

BOOST_AUTO_TEST_CASE( basics )
{
    std::vector<double> weights = {0.3, 0.3, 0.4};
    const std::shared_ptr<Classifier> pC = std::make_shared<ClassifierWeightedSum>(weights);
    BOOST_CHECK_EQUAL(pC->dim(), 3);
    BOOST_CHECK_EQUAL(pC->confidence(std::vector<double>({1,0,0})), 0.3);
    BOOST_CHECK_EQUAL(pC->confidence(std::vector<double>({0,1,0})), 0.3);
    BOOST_CHECK_EQUAL(pC->confidence(std::vector<double>({0,0,1})), 0.4);
    BOOST_CHECK_EQUAL(pC->confidence(std::vector<double>({0.3,0.2,0.1})), 0.3*0.3+0.2*0.3+0.4*0.1);
}

BOOST_AUTO_TEST_SUITE_END()
