#define BOOST_TEST_MODULE lib_test_sample_creator
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"

BOOST_AUTO_TEST_CASE( from_stream ) {
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
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);

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
    std::stringstream ss_buffer((std::stringstream(data_buffer)));

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator.from_stream(ss_buffer);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->get_dim(), 4);
    BOOST_CHECK_EQUAL(pSample->get_size(), 4);
    BOOST_CHECK((*pSample)[0]->get_data() == std::vector<double>({11,22,44,77}) );
    BOOST_CHECK((*pSample)[1]->get_data() == std::vector<double>({12,22,44,77}) );
    BOOST_CHECK((*pSample)[2]->get_data() == std::vector<double>({14,22,44,77}) );
    BOOST_CHECK((*pSample)[3]->get_data() == std::vector<double>({11,23,44,77}) );
    BOOST_CHECK_EQUAL((*pSample)[0]->get_weight_negatives(), 4);
    BOOST_CHECK_EQUAL((*pSample)[0]->get_weight_positives(), 1);
    BOOST_CHECK_EQUAL((*pSample)[1]->get_weight_negatives(), 2);
    BOOST_CHECK_EQUAL((*pSample)[1]->get_weight_positives(), 0);
    BOOST_CHECK_EQUAL((*pSample)[2]->get_weight_negatives(), 0);
    BOOST_CHECK_EQUAL((*pSample)[2]->get_weight_positives(), 3);
    BOOST_CHECK_EQUAL((*pSample)[3]->get_weight_negatives(), 0);
    BOOST_CHECK_EQUAL((*pSample)[3]->get_weight_positives(), 18);
}

BOOST_AUTO_TEST_CASE( from_file ) {
    const std::string names_file("data/tmc_paper_9/tmc_paper.names");
    const std::string data_file("data/tmc_paper_9/tmc_paper.data");

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->get_dim(), 2);
    BOOST_CHECK_EQUAL(pSample->get_size(), 4);
}

BOOST_AUTO_TEST_CASE( from_sample ) {
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
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);

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
    std::stringstream ss_buffer((std::stringstream(data_buffer)));

    SampleCreator sample_creator1;
    sample_creator1.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator1.from_stream(ss_buffer);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);


    BOOST_TEST_MESSAGE("Creating SubSample of a sample with indices {1,3}");
    SampleCreator sample_creator2;
    std::shared_ptr<Sample> pSubSample = sample_creator2.from_sample(pSample, std::vector<unsigned int>({1,3}));
    BOOST_TEST_MESSAGE("Resulting SubSample: " << *pSubSample);
    BOOST_CHECK((*pSample)[3]->get_data() == (*pSubSample)[1]->get_data());
    BOOST_CHECK_EQUAL(pSample->get_dim(), pSubSample->get_dim());
    BOOST_CHECK_EQUAL(2, pSubSample->get_size());
}

BOOST_AUTO_TEST_CASE( test_neg_pos_count ) {
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
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);

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
    std::stringstream ss_buffer((std::stringstream(data_buffer)));

    SampleCreator sample_creator1;
    sample_creator1.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator1.from_stream(ss_buffer);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);


    BOOST_TEST_MESSAGE("Creating SubSample of a sample with indices {1,3,2}");
    SampleCreator sample_creator2;
    std::shared_ptr<Sample> pSubSample = sample_creator2.from_sample(pSample, std::vector<unsigned int>({1,3,2}));
    BOOST_TEST_MESSAGE("Resulting SubSample: " << *pSubSample);
    BOOST_CHECK( pSample->get_neg_pos_counts() == (std::pair<double, double>(6,22)));
    BOOST_CHECK( pSubSample->get_neg_pos_counts() == (std::pair<double, double>(2,21)));
}
