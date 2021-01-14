#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_sample
#endif
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "sample.h"

#ifndef TMC_UNIT_TESTS
bool is_critical(const std::exception& ) { return true; }
#endif

BOOST_AUTO_TEST_SUITE( sample )

BOOST_AUTO_TEST_CASE( basics ) {
    Sample sample(4);
    BOOST_CHECK_EQUAL(  sample.dim(), 4 );

    std::shared_ptr<FeatureVector> pFV1 =
            std::make_shared<FeatureVector>("11,22,33,44,v2,5,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    BOOST_CHECK_EQUAL(  pFV1.use_count(), 1 );
    BOOST_TEST_MESSAGE("Pushing pFV1: " << *pFV1);
    sample.push(pFV1);
    BOOST_TEST_MESSAGE("After pushing a feature vector, checking shared pointers to the object");
    BOOST_CHECK_EQUAL(  pFV1.use_count(), 2 );
    BOOST_CHECK_EQUAL(  sample.size(), 1 );
    std::shared_ptr<FeatureVector> pFV2 =
            std::make_shared<FeatureVector>("11,22,33,44,v1,6,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV3 =
            std::make_shared<FeatureVector>("12,22,33,44,v2,7,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV4 =
            std::make_shared<FeatureVector>("11,22,34,44,v2,8,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    BOOST_TEST_MESSAGE("Pushing pFV2: " << *pFV2);
    sample.push(pFV2);
    BOOST_TEST_MESSAGE("Pushing pFV3: " << *pFV3);
    sample.push(pFV3);
    BOOST_TEST_MESSAGE("Pushing pFV4: " << *pFV4);
    sample.push(pFV4);
    BOOST_TEST_MESSAGE("Resulting sample: " << sample);
    BOOST_CHECK_EQUAL(sample.size(), 2);
    BOOST_CHECK_EQUAL(sample.dim(), 4);
    BOOST_CHECK_EQUAL(sample[0]->get_weight_negatives(), 6);
    BOOST_CHECK_EQUAL(sample[0]->get_weight_positives(), 13);
    BOOST_CHECK_EQUAL(sample[1]->get_weight_negatives(), 0);
    BOOST_CHECK_EQUAL(sample[1]->get_weight_positives(), 7);
    BOOST_CHECK(sample[0]->get_data() == std::vector<double>({11,22,44,77}) );
    BOOST_CHECK(sample[1]->get_data() == std::vector<double>({12,22,44,77}) );
    BOOST_CHECK(sample.get_neg_pos_counts() == (std::pair<double, double>(6,20)) );
    std::shared_ptr<FeatureVector> pFV5 =
            std::make_shared<FeatureVector>("987,22,34,44,v2,8,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    BOOST_CHECK_EQUAL(sample.contains(pFV5), false);
    BOOST_CHECK_EQUAL(sample.contains(pFV1), true);
    BOOST_CHECK_EQUAL(sample.contains(pFV2), true);
    BOOST_CHECK_EQUAL(sample.contains(pFV3), true);
    BOOST_CHECK_EQUAL(sample.contains(pFV4), true);

    /*
    BOOST_TEST_MESSAGE("Pushing pFV3 without check: " << *pFV3);
    sample.push_no_check(pFV3);
    BOOST_TEST_MESSAGE("Resulting sample: " << sample);
    BOOST_TEST_MESSAGE("Pushing pFV3 with check (expecting exception): " << *pFV3);
    BOOST_CHECK_EXCEPTION( sample.push(pFV3), std::domain_error, is_critical);
     */
}

BOOST_AUTO_TEST_CASE( ptree ) {
    Sample sample(4);

    std::shared_ptr<FeatureVector> pFV1 =
            std::make_shared<FeatureVector>("11,22,33,44,v2,5,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV2 =
            std::make_shared<FeatureVector>("11,22,33,44,v1,6,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV3 =
            std::make_shared<FeatureVector>("12,22,33,44,v2,7,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV4 =
            std::make_shared<FeatureVector>("11,22,34,44,v2,8,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    sample.push(pFV1);
    sample.push(pFV2);
    sample.push(pFV3);
    sample.push(pFV4);
    BOOST_TEST_MESSAGE("Sample: " << sample);

    boost::property_tree::ptree pt;
    sample.dump_to_ptree(pt);

    BOOST_CHECK_EQUAL(pt.size(), 5);

    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    Sample sample1(pt);
    BOOST_TEST_MESSAGE("Read from ptree: " << sample1);
    BOOST_CHECK_EQUAL(sample.dim(), sample1.dim());
    BOOST_CHECK_EQUAL(sample.size(), sample1.size());
    BOOST_CHECK(sample.get_neg_pos_counts() == sample1.get_neg_pos_counts());
    BOOST_CHECK(!sample.has_no_intersection_with(sample1));
    BOOST_CHECK(!sample1.has_no_intersection_with(sample));
}

BOOST_AUTO_TEST_CASE( weights_int ) {
    Sample sample(4);

    std::shared_ptr<FeatureVector> pFV1 =
            std::make_shared<FeatureVector>("11,22,33,44,v2,5,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV2 =
            std::make_shared<FeatureVector>("11,22,33,44,v1,6,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV3 =
            std::make_shared<FeatureVector>("12,22,33,44,v2,7,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV4 =
            std::make_shared<FeatureVector>("11,22,34,44,v2,8.5,77",
                                            std::vector<size_t>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    sample.push(pFV1);
    sample.push(pFV2);
    sample.push(pFV3);
    BOOST_CHECK(sample.weights_int());
    sample.push(pFV4);
    BOOST_CHECK(!sample.weights_int());
    BOOST_TEST_MESSAGE("Sample: " << sample);
}

BOOST_AUTO_TEST_SUITE_END()