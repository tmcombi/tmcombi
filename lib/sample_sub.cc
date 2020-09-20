#define BOOST_TEST_MODULE lib_test_sample_sub
#include <boost/test/included/unit_test.hpp>

#include "sample.h"

BOOST_AUTO_TEST_CASE( test_SubSample ) {
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
    std::shared_ptr<SampleContainer> pSample = std::make_shared<SampleContainer>(pFN);
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
    pSample->push_from_stream(ss_buffer);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_TEST_MESSAGE("Creating SubSample of a sample with indices {1,3}");
    std::shared_ptr<SubSample> pSubSample =
            std::make_shared<SubSample>(pSample, std::vector<unsigned int>({1,3}));
    BOOST_TEST_MESSAGE("Resulting subsample: " << *pSubSample);
    BOOST_CHECK((*pSample)[3].get_data() == (*pSubSample)[1].get_data());
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
    std::shared_ptr<SampleContainer> pSample = std::make_shared<SampleContainer>(pFN);
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
    pSample->push_from_stream(ss_buffer);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_TEST_MESSAGE("Creating SubSample of a sample with indices {1,3,2}");
    std::shared_ptr<SubSample> pSubSample =
            std::make_shared<SubSample>(pSample, std::vector<unsigned int>({1,3,2}));
    BOOST_TEST_MESSAGE("Resulting subsample: " << *pSubSample);
    BOOST_CHECK( pSample->get_neg_pos() == (std::pair<double, double>(6,22)));
    BOOST_CHECK( pSubSample->get_neg_pos() == (std::pair<double, double>(2,21)));
}
