#define BOOST_TEST_MODULE lib_test_sample
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "sample.h"

bool is_critical(const std::exception& ex ) { return true; }

BOOST_AUTO_TEST_CASE( sample_basics ) {
    Sample sample(4);
    BOOST_CHECK_EQUAL(  sample.get_dim(), 4 );

    std::shared_ptr<FeatureVector> pFV1 =
            std::make_shared<FeatureVector>("11,22,33,44,v2,5,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    BOOST_CHECK_EQUAL(  pFV1.use_count(), 1 );
    BOOST_TEST_MESSAGE("Pushing pFV1: " << *pFV1);
    sample.push(pFV1);
    BOOST_TEST_MESSAGE("After pushing a feature vector, checking shared pointers to the object");
    BOOST_CHECK_EQUAL(  pFV1.use_count(), 2 );
    BOOST_CHECK_EQUAL(  sample.get_size(), 1 );
    std::shared_ptr<FeatureVector> pFV2 =
            std::make_shared<FeatureVector>("11,22,33,44,v1,6,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV3 =
            std::make_shared<FeatureVector>("12,22,33,44,v2,7,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV4 =
            std::make_shared<FeatureVector>("11,22,34,44,v2,8,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
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
    BOOST_CHECK_EQUAL(sample.get_size(), 2);
    BOOST_CHECK_EQUAL(sample.get_dim(), 4);
    BOOST_CHECK_EQUAL(sample[0]->get_weight_negatives(), 6);
    BOOST_CHECK_EQUAL(sample[0]->get_weight_positives(), 13);
    BOOST_CHECK_EQUAL(sample[1]->get_weight_negatives(), 0);
    BOOST_CHECK_EQUAL(sample[1]->get_weight_positives(), 7);
    BOOST_CHECK(sample[0]->get_data() == std::vector<double>({11,22,44,77}) );
    BOOST_CHECK(sample[1]->get_data() == std::vector<double>({12,22,44,77}) );
    BOOST_CHECK(sample.get_neg_pos_counts() == (std::pair<double, double>(6,20)) );
    std::shared_ptr<FeatureVector> pFV5 =
            std::make_shared<FeatureVector>("987,22,34,44,v2,8,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    BOOST_CHECK_EQUAL(sample.contains(pFV5), false);
    BOOST_CHECK_EQUAL(sample.contains(pFV1), true);
    BOOST_CHECK_EQUAL(sample.contains(pFV2), true);
    BOOST_CHECK_EQUAL(sample.contains(pFV3), true);
    BOOST_CHECK_EQUAL(sample.contains(pFV4), true);

    BOOST_TEST_MESSAGE("Pushing pFV3 without check: " << *pFV3);
    sample.push_no_check(pFV3);
    BOOST_TEST_MESSAGE("Resulting sample: " << sample);
    BOOST_TEST_MESSAGE("Pushing pFV3 with check (expecting exception): " << *pFV3);
    BOOST_CHECK_EXCEPTION( sample.push(pFV3), std::domain_error, is_critical);
}

BOOST_AUTO_TEST_CASE( sample_dump_to_ptree ) {
    Sample sample(4);

    std::shared_ptr<FeatureVector> pFV1 =
            std::make_shared<FeatureVector>("11,22,33,44,v2,5,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV2 =
            std::make_shared<FeatureVector>("11,22,33,44,v1,6,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV3 =
            std::make_shared<FeatureVector>("12,22,33,44,v2,7,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
                                            4,
                                            "v1",
                                            "v2",
                                            5);
    std::shared_ptr<FeatureVector> pFV4 =
            std::make_shared<FeatureVector>("11,22,34,44,v2,8,77",
                                            std::vector<unsigned int>({0, 1, 3, 6}),
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
}
