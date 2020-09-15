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
    BOOST_TEST_MESSAGE("Testing sample from names buffer:\n"
                               << "#######################################################\n"
                               << buffer
                               << "#######################################################" );
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(std::stringstream(buffer));
    Sample sample(pFN);
    BOOST_CHECK_EQUAL(  sample.get_dim(), pFN->get_dim() );
    BOOST_TEST_MESSAGE("After creating a Sample, checking shared pointers to the object FeatureNames");
    BOOST_CHECK_EQUAL(  pFN.use_count(), 2 );
    std::shared_ptr<FeatureVector> pFV1 = std::make_shared<FeatureVector>("11,22,33,44,v2,5,77",
                                                                          pFN->get_feature_indices(),
                                                                          pFN->get_target_feature_index(),
                                                                          pFN->get_negatives_label(),
                                                                          pFN->get_positives_label(),
                                                                          pFN->get_weight_index());
    BOOST_TEST_MESSAGE("Pushing pFV1: " << *pFV1);
    sample.push(pFV1);
    BOOST_CHECK_EQUAL(  sample.get_size(), 1 );
    std::shared_ptr<FeatureVector> pFV2 = std::make_shared<FeatureVector>("11,22,33,44,v1,6,77",
                                                                          pFN->get_feature_indices(),
                                                                          pFN->get_target_feature_index(),
                                                                          pFN->get_negatives_label(),
                                                                          pFN->get_positives_label(),
                                                                          pFN->get_weight_index());
    std::shared_ptr<FeatureVector> pFV3 = std::make_shared<FeatureVector>("12,22,33,44,v2,7,77",
                                                                          pFN->get_feature_indices(),
                                                                          pFN->get_target_feature_index(),
                                                                          pFN->get_negatives_label(),
                                                                          pFN->get_positives_label(),
                                                                          pFN->get_weight_index());
    std::shared_ptr<FeatureVector> pFV4 = std::make_shared<FeatureVector>("11,22,34,44,v2,8,77",
                                                                          pFN->get_feature_indices(),
                                                                          pFN->get_target_feature_index(),
                                                                          pFN->get_negatives_label(),
                                                                          pFN->get_positives_label(),
                                                                          pFN->get_weight_index());
    BOOST_TEST_MESSAGE("Pushing pFV2: " << *pFV2);
    sample.push(pFV2);
    BOOST_TEST_MESSAGE("Pushing pFV3: " << *pFV3);
    sample.push(pFV3);
    BOOST_TEST_MESSAGE("Pushing pFV4: " << *pFV4);
    sample.push(pFV4);
    BOOST_TEST_MESSAGE("Resulting sample: " << sample);
    BOOST_CHECK_EQUAL(sample.get_size(), 2);
    BOOST_CHECK_EQUAL(sample.get_dim(), 4);
    BOOST_CHECK_EQUAL(sample[0].get_weight_negatives(), 6);
    BOOST_CHECK_EQUAL(sample[0].get_weight_positives(), 13);
    BOOST_CHECK_EQUAL(sample[1].get_weight_negatives(), 0);
    BOOST_CHECK_EQUAL(sample[1].get_weight_positives(), 7);
    BOOST_CHECK(sample[0].get_data() == std::vector<double>({11,22,44,77}) );
    BOOST_CHECK(sample[1].get_data() == std::vector<double>({12,22,44,77}) );
}

BOOST_AUTO_TEST_CASE( push_data_from_stream ) {
    std::string names_buffer("| this is comment\n"
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
    BOOST_TEST_MESSAGE("Creating sample from names buffer:\n"
                               << "#######################################################\n"
                               << names_buffer
                               << "#######################################################");
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(std::stringstream(names_buffer));
    Sample sample(pFN);
    std::string data_buffer("11,22,34,44,v2,1,77\n"
                            "12,22,34,44,v1,2,77\n"
                            "14,22,34,44,v2,3,77\n"
                            "11,22,34,44,v1,4,77\n"
                            "11,23,34,44,v2,5,77\n"
                            "11,23,34,44,v2,6,77\n"
                            "11,23,34,44,v2,7,77\n"
                            );
    BOOST_TEST_MESSAGE("Pushing data from buffer:\n"
                               << "#######################################################\n"
                               << data_buffer
                               << "#######################################################");
    sample.push_from_stream(std::stringstream(data_buffer));
    BOOST_TEST_MESSAGE("Resulting sample: " << sample);
    BOOST_CHECK_GT(sample.get_dim(), 0);
}