#define BOOST_TEST_MODULE lib_test_sample_creator
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"

BOOST_AUTO_TEST_CASE( sample_creator_from_stream ) {
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

BOOST_AUTO_TEST_CASE( sample_creator_from_file ) {
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

BOOST_AUTO_TEST_CASE( sample_creator_from_sample ) {
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
    //todo: write one more with dynamic_bitset
    std::shared_ptr<Sample> pSubSample = sample_creator2.from_sample(pSample, std::vector<unsigned int>({1,3}));
    BOOST_TEST_MESSAGE("Resulting SubSample: " << *pSubSample);
    BOOST_CHECK((*pSample)[3]->get_data() == (*pSubSample)[1]->get_data());
    BOOST_CHECK_EQUAL(pSample->get_dim(), pSubSample->get_dim());
    BOOST_CHECK_EQUAL(2, pSubSample->get_size());
}

BOOST_AUTO_TEST_CASE( sample_creator_neg_pos_count ) {
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

BOOST_AUTO_TEST_CASE( sample_creator_comparison_and_merge ) {
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
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);

    std::string data_buffer1("11,22,34,44,v2,1,77\n"
                            "12,22,34,44,v1,2,77\n"
                            "14,22,34,44,v2,3,77\n"
                            );
    std::string data_buffer2("14,22,34,44,v2,3,77\n"
                             "11,22,34,43,v1,4,77\n"
                             );
    std::string data_buffer3("14,22,34,45,v2,3,77\n"
                             "11,22,34,45,v1,4,77\n"
                             "11,23,34,44,v2,5,77\n"
                             );
    std::string data_buffer4("15,22,34,45,v2,3,77\n"
                             "12,23,34,45,v1,4,77\n"
                             "13,23,4,44,v2,5,77\n"
                             );

    std::stringstream ss_buffer1((std::stringstream(data_buffer1)));
    std::stringstream ss_buffer2((std::stringstream(data_buffer2)));
    std::stringstream ss_buffer3((std::stringstream(data_buffer3)));
    std::stringstream ss_buffer4((std::stringstream(data_buffer4)));

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample1 = sample_creator.from_stream(ss_buffer1);
    std::shared_ptr<Sample> pSample2 = sample_creator.from_stream(ss_buffer2);
    std::shared_ptr<Sample> pSample3 = sample_creator.from_stream(ss_buffer3);
    std::shared_ptr<Sample> pSample4 = sample_creator.from_stream(ss_buffer4);
    BOOST_TEST_MESSAGE("Sample1: " << *pSample1);
    BOOST_TEST_MESSAGE("Sample2: " << *pSample2);
    BOOST_TEST_MESSAGE("Sample3: " << *pSample3);
    BOOST_TEST_MESSAGE("Sample4: " << *pSample4);

    BOOST_CHECK_EQUAL(pSample2->has_no_intersection_with(*pSample1), false);
    BOOST_CHECK_EQUAL(pSample1->has_no_intersection_with(*pSample2), false);
    BOOST_CHECK_EQUAL(pSample3->has_no_intersection_with(*pSample1), true);
    BOOST_CHECK_EQUAL(pSample1->has_no_intersection_with(*pSample3), true);
    BOOST_CHECK_GE( *pSample4, *pSample3 );
    BOOST_CHECK_LE( *pSample3, *pSample4 );
    BOOST_CHECK( !(*pSample4 <= *pSample3) );
    BOOST_CHECK( !(*pSample3 >= *pSample4) );
    BOOST_CHECK( !(*pSample1 <= *pSample2) );
    BOOST_CHECK( !(*pSample1 >= *pSample2) );
    BOOST_CHECK( !(*pSample2 <= *pSample1) );
    BOOST_CHECK( !(*pSample2 >= *pSample1) );

    std::shared_ptr<Sample> pSample12 = sample_creator.merge(pSample1, pSample2);
    BOOST_TEST_MESSAGE("Merged Sample12: " << *pSample12);
    BOOST_CHECK_EQUAL(pSample12->get_size(), 4);
}

BOOST_AUTO_TEST_CASE( sample_creator_borders ) {
    std::string names_buffer("| this is comment\n"
                             "target_feature.| one more comment\n"
                             "\n"
                             "   feature1: continuous.   \n"
                             "feature2: continuous.   | third comment\n"
                             "feature3: ignore.\n"
                             "feature4: ignore.\n"
                             "target_feature: v1, v2.\n"
                             "case weight: continuous.\n"
                             "feature5: ignore.\n"
                             "\n"
                             "  | one more comment here\n"
                             "\n");
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);

    std::string data_buffer1("1,8,34,44,v2,1,77\n"
                             "2,2,34,44,v1,2,77\n"
                             "2,5,34,44,v2,3,77\n"
                             "3,7,34,44,v2,1,77\n"
                             "3,9,34,44,v1,2,77\n"
                             "4,1,34,44,v2,3,77\n"
                             "4,5,34,44,v2,1,77\n"
                             "5,8,34,44,v1,2,77\n"
                             "6,0,34,44,v2,3,77\n"
                             "7,1,34,44,v2,3,77\n"
                             "7,5,34,44,v2,3,77\n"
                             "8,2,34,44,v2,3,77\n"
    );

    std::stringstream ss_buffer1((std::stringstream(data_buffer1)));

    SampleCreator sample_creator1;
    sample_creator1.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample1 = sample_creator1.from_stream(ss_buffer1);
    BOOST_TEST_MESSAGE("Sample1: " << *pSample1);


    std::shared_ptr<Sample> pLower;
    std::shared_ptr<Sample> pUpper;

    SampleCreator sample_creator2;
    std::tie(pLower,pUpper) = sample_creator2.borders(pSample1);
    BOOST_TEST_MESSAGE("Lower border: " << *pLower);
    BOOST_TEST_MESSAGE("Upper border: " << *pUpper);

    BOOST_CHECK(pSample1->get_dim() ==pLower->get_dim());
    BOOST_CHECK(pSample1->get_neg_pos_counts() ==pLower->get_neg_pos_counts());
    BOOST_CHECK(pSample1->get_neg_pos_counts() ==pUpper->get_neg_pos_counts());

    BOOST_CHECK_EQUAL(pLower->get_size(), 4);
    BOOST_CHECK_EQUAL(pUpper->get_size(), 4);

    BOOST_CHECK(pLower->contains((*pSample1)[0]));
    BOOST_CHECK(pLower->contains((*pSample1)[1]));
    BOOST_CHECK(pLower->contains((*pSample1)[5]));
    BOOST_CHECK(pLower->contains((*pSample1)[8]));

    BOOST_CHECK(pUpper->contains((*pSample1)[4]));
    BOOST_CHECK(pUpper->contains((*pSample1)[7]));
    BOOST_CHECK(pUpper->contains((*pSample1)[10]));
    BOOST_CHECK(pUpper->contains((*pSample1)[11]));

    BOOST_CHECK_GE(*pUpper, *pLower);
    BOOST_CHECK_LE(*pLower, *pUpper);
    BOOST_CHECK(!(*pUpper <= *pLower));
    BOOST_CHECK(!(*pLower >= *pUpper));
    BOOST_CHECK(pLower->has_no_intersection_with(*pUpper));
    BOOST_CHECK(pUpper->has_no_intersection_with(*pLower));
}

BOOST_AUTO_TEST_CASE( sample_creator_split ) {
    std::string names_buffer("| this is comment\n"
                             "target_feature.| one more comment\n"
                             "\n"
                             "   feature1: continuous.   \n"
                             "feature2: continuous.   | third comment\n"
                             "feature3: ignore.\n"
                             "feature4: ignore.\n"
                             "target_feature: v1, v2.\n"
                             "case weight: continuous.\n"
                             "feature5: ignore.\n"
                             "\n"
                             "  | one more comment here\n"
                             "\n");
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);
    std::string data_buffer1("1,8,34,44,v2,1,77\n"
                             "2,2,34,44,v1,2,77\n"
                             "2,5,34,44,v2,3,77\n"
                             "3,7,34,44,v2,1,77\n"
                             "3,9,34,44,v1,2,77\n"
                             "4,1,34,44,v2,3,77\n"
                             "4,5,34,44,v2,1,77\n"
                             "5,8,34,44,v1,2,77\n"
                             "6,0,34,44,v2,3,77\n"
                             "7,1,34,44,v2,3,77\n"
                             "7,5,34,44,v2,3,77\n"
                             "8,2,34,44,v2,3,77\n"
                             );
    std::stringstream ss_buffer1((std::stringstream(data_buffer1)));

    SampleCreator sample_creator1;
    sample_creator1.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample1 = sample_creator1.from_stream(ss_buffer1);
    BOOST_TEST_MESSAGE("Sample1: " << *pSample1);

    std::shared_ptr<Sample> pLower;
    std::shared_ptr<Sample> pUpper;

    boost::dynamic_bitset<> db(pSample1->get_size());
    db[3] = db[4] = db[7] = true;

    SampleCreator sample_creator2;
    std::tie(pLower,pUpper) = sample_creator2.split_sample(pSample1, db);
    BOOST_TEST_MESSAGE("Lower subsample: " << *pLower);
    BOOST_TEST_MESSAGE("Upper subsample: " << *pUpper);

    BOOST_CHECK(pSample1->get_dim() == pLower->get_dim());
    BOOST_CHECK_EQUAL(pUpper->get_size(),3);
    BOOST_CHECK_EQUAL(pUpper->get_size() + pLower->get_size(), pSample1->get_size());
    BOOST_CHECK_EQUAL(pUpper->get_neg_pos_counts().first + pLower->get_neg_pos_counts().first , pSample1->get_neg_pos_counts().first );
    BOOST_CHECK_EQUAL(pUpper->get_neg_pos_counts().second  + pLower->get_neg_pos_counts().second, pSample1->get_neg_pos_counts().second);

    BOOST_CHECK(pLower->contains((*pSample1)[0]));
    BOOST_CHECK(pLower->contains((*pSample1)[1]));
    BOOST_CHECK(pLower->contains((*pSample1)[5]));
    BOOST_CHECK(pLower->contains((*pSample1)[6]));

    BOOST_CHECK(pUpper->contains((*pSample1)[4]));
    BOOST_CHECK(pUpper->contains((*pSample1)[7]));
    BOOST_CHECK(pUpper->contains((*pSample1)[3]));

    BOOST_CHECK_GE(*pUpper, *pLower);
    BOOST_CHECK_LE(*pLower, *pUpper);
    BOOST_CHECK(!(*pUpper <= *pLower));
    BOOST_CHECK(!(*pLower >= *pUpper));
    BOOST_CHECK(pLower->has_no_intersection_with(*pUpper));
    BOOST_CHECK(pUpper->has_no_intersection_with(*pLower));
}