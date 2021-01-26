#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_feature_transform_superposition
#endif
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "feature_transform_superposition.h"

BOOST_AUTO_TEST_SUITE( feature_transform_superposition )

BOOST_AUTO_TEST_CASE( basics ) {
    std::string names_file, data_file;

    names_file = "data/4layers_36points/4layers_36points.names";
    data_file  = "data/4layers_36points/4layers_36points.data";
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN36 = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator36;
    sample_creator36.set_feature_names(pFN36);
    std::shared_ptr<Sample> pSample36 = sample_creator36.from_file(data_file);
    BOOST_CHECK_EQUAL(pSample36->dim(), 2);
    BOOST_CHECK_EQUAL(pSample36->size(), 36);
    std::shared_ptr<ClassifierCreatorTrain> pTC36 = std::make_shared<ClassifierCreatorTrainTmc>();
    (*pTC36).init(pSample36).train();
    std::shared_ptr<Classifier> pClTmc36 = pTC36->get_classifier();

    names_file = "data/tmc_paper_9/tmc_paper.names";
    data_file  = "data/tmc_paper_9/tmc_paper.data";
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN9 = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator9;
    sample_creator9.set_feature_names(pFN9);
    std::shared_ptr<Sample> pSample9 = sample_creator9.from_file(data_file);
    BOOST_CHECK_EQUAL(pSample9->dim(), 2);
    BOOST_CHECK_EQUAL(pSample9->size(), 4);
    std::shared_ptr<ClassifierCreatorTrain> pTC9 = std::make_shared<ClassifierCreatorTrainTmc>();
    (*pTC9).init(pSample9).train();
    std::shared_ptr<Classifier> pClTmc9 = pTC9->get_classifier();

    pSample36->push(pSample9);
    BOOST_CHECK_EQUAL(pSample36->dim(), 2);
    BOOST_CHECK_EQUAL(pSample36->size(), 40);
    std::shared_ptr<ClassifierCreatorTrain> pTCcombined = std::make_shared<ClassifierCreatorTrainTmc>();
    (*pTCcombined).init(pSample36).train();
    std::shared_ptr<Classifier> pClTmcCombined = pTCcombined->get_classifier();

    const std::vector<std::shared_ptr<Classifier>> classifiers = {pClTmcCombined, pClTmc36, pClTmc9};
    std::shared_ptr<FeatureTransformSuperposition> pFT1 = std::make_shared<FeatureTransformSuperposition>();
    pFT1->set_classifiers(classifiers);
    std::shared_ptr<FeatureTransform> pFT2 = std::make_shared<FeatureTransformSuperposition>(classifiers);
    BOOST_CHECK_EQUAL(pFT1->dim_in(),2);BOOST_CHECK_EQUAL(pFT2->dim_in(),2);
    BOOST_CHECK_EQUAL(pFT1->dim_out(),3);BOOST_CHECK_EQUAL(pFT2->dim_out(),3);
    std::vector<double> p, v_out;
    p = {10,2};
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 1.0/6.0);
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 1.0/6.0);
    p = {16,1};
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 1.0/3.0);
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 1.0/3.0);
    p = {7,10};
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 2.0/3.0);
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 2.0/3.0);
    p = {11,10};
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 5.0/6.0);
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 5.0/6.0);
    p = {4,6};
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 1.0/6.0);
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 1.0/6.0);
    p = {7,13};
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 5.0/6.0);
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], 5.0/6.0);
    p = {14.5,0.5};
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8)/(double)(50+10+16+8));
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8)/(double)(50+10+16+8));
    p = {-1,16};
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {-1,18};
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {12.5,1.5};
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8)/(double)(50+10+16+8));
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8)/(double)(50+10+16+8));
    p = {4,8};
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18)/(double)(50+10+16+8+9+18));
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18)/(double)(50+10+16+8+9+18));
    p = {3,13};
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {1,18};
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {15,5.5};
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (8+18+45)/(double)(16+8+9+18+9+45));
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (8+18+45)/(double)(16+8+9+18+9+45));
    p = {18,1};
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (8+18+45)/(double)(16+8+9+18+9+45));
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (8+18+45)/(double)(16+8+9+18+9+45));
    p = {8,12};
    pFT2->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (18+45)/(double)(9+18+9+45));
    pFT1->transform_std_vector(p, v_out); BOOST_CHECK_EQUAL(v_out[1], (18+45)/(double)(9+18+9+45));
}

BOOST_AUTO_TEST_CASE( ptree ) {
    std::string names_file, data_file;

    names_file = "data/4layers_36points/4layers_36points.names";
    data_file = "data/4layers_36points/4layers_36points.data";
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN36 = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator36;
    sample_creator36.set_feature_names(pFN36);
    std::shared_ptr<Sample> pSample36 = sample_creator36.from_file(data_file);
    BOOST_CHECK_EQUAL(pSample36->dim(), 2);
    BOOST_CHECK_EQUAL(pSample36->size(), 36);
    std::shared_ptr<ClassifierCreatorTrain> pTC36 = std::make_shared<ClassifierCreatorTrainTmc>();
    (*pTC36).init(pSample36).train();
    std::shared_ptr<Classifier> pClTmc36 = pTC36->get_classifier();

    names_file = "data/tmc_paper_9/tmc_paper.names";
    data_file = "data/tmc_paper_9/tmc_paper.data";
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN9 = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator9;
    sample_creator9.set_feature_names(pFN9);
    std::shared_ptr<Sample> pSample9 = sample_creator9.from_file(data_file);
    BOOST_CHECK_EQUAL(pSample9->dim(), 2);
    BOOST_CHECK_EQUAL(pSample9->size(), 4);
    std::shared_ptr<ClassifierCreatorTrain> pTC9 = std::make_shared<ClassifierCreatorTrainTmc>();
    (*pTC9).init(pSample9).train();
    std::shared_ptr<Classifier> pClTmc9 = pTC9->get_classifier();

    pSample36->push(pSample9);
    BOOST_CHECK_EQUAL(pSample36->dim(), 2);
    BOOST_CHECK_EQUAL(pSample36->size(), 40);
    std::shared_ptr<ClassifierCreatorTrain> pTCcombined = std::make_shared<ClassifierCreatorTrainTmc>();
    (*pTCcombined).init(pSample36).train();
    std::shared_ptr<Classifier> pClTmcCombined = pTCcombined->get_classifier();

    const std::vector<std::shared_ptr<Classifier>> classifiers = {pClTmcCombined, pClTmc36, pClTmc9};
    std::shared_ptr<FeatureTransform> pFT1 = std::make_shared<FeatureTransformSuperposition>(classifiers);
    boost::property_tree::ptree pt;
    pFT1->dump_to_ptree(pt);
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    std::vector<std::shared_ptr<Classifier>> classifiers2;
    BOOST_CHECK_EQUAL("FeatureTransformSuperposition", pt.get<std::string>("type"));
    BOOST_CHECK_EQUAL(pt.get<size_t>("dim_in"),2);
    BOOST_CHECK_EQUAL(pt.get<size_t>("dim_out"),3);
    for (auto& item : pt.get_child("classifiers")) {
        std::shared_ptr<Classifier> pC = ClassifierCreatorDispatchPtree(item.second).get_classifier();
        BOOST_CHECK_EQUAL(pC->dim(),2);
        classifiers2.push_back(pC);
    }
    BOOST_CHECK_EQUAL(classifiers2.size(),3);

    std::shared_ptr<FeatureTransform> pFT2 = std::make_shared<FeatureTransformSuperposition>(classifiers2);
    BOOST_CHECK_EQUAL(pFT1->dim_in(),2);BOOST_CHECK_EQUAL(pFT2->dim_in(),2);
    BOOST_CHECK_EQUAL(pFT1->dim_out(),3);BOOST_CHECK_EQUAL(pFT2->dim_out(),3);
    std::vector<double> p, v_out;
    p = {10, 2};
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 1.0 / 6.0);
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 1.0 / 6.0);
    p = {16, 1};
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 1.0 / 3.0);
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 1.0 / 3.0);
    p = {7, 10};
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 2.0 / 3.0);
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 2.0 / 3.0);
    p = {11, 10};
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 5.0 / 6.0);
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 5.0 / 6.0);
    p = {4, 6};
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 1.0 / 6.0);
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 1.0 / 6.0);
    p = {7, 13};
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 5.0 / 6.0);
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], 5.0 / 6.0);
    p = {14.5, 0.5};
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8) / (double) (50 + 10 + 16 + 8));
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8) / (double) (50 + 10 + 16 + 8));
    p = {-1, 16};
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18 + 45) / (double) (50 + 10 + 16 + 8 + 9 + 18 + 9 + 45));
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18 + 45) / (double) (50 + 10 + 16 + 8 + 9 + 18 + 9 + 45));
    p = {-1, 18};
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18 + 45) / (double) (50 + 10 + 16 + 8 + 9 + 18 + 9 + 45));
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18 + 45) / (double) (50 + 10 + 16 + 8 + 9 + 18 + 9 + 45));
    p = {12.5, 1.5};
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8) / (double) (50 + 10 + 16 + 8));
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8) / (double) (50 + 10 + 16 + 8));
    p = {4, 8};
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18) / (double) (50 + 10 + 16 + 8 + 9 + 18));
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18) / (double) (50 + 10 + 16 + 8 + 9 + 18));
    p = {3, 13};
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18 + 45) / (double) (50 + 10 + 16 + 8 + 9 + 18 + 9 + 45));
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18 + 45) / (double) (50 + 10 + 16 + 8 + 9 + 18 + 9 + 45));
    p = {1, 18};
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18 + 45) / (double) (50 + 10 + 16 + 8 + 9 + 18 + 9 + 45));
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (10 + 8 + 18 + 45) / (double) (50 + 10 + 16 + 8 + 9 + 18 + 9 + 45));
    p = {15, 5.5};
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (8 + 18 + 45) / (double) (16 + 8 + 9 + 18 + 9 + 45));
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (8 + 18 + 45) / (double) (16 + 8 + 9 + 18 + 9 + 45));
    p = {18, 1};
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (8 + 18 + 45) / (double) (16 + 8 + 9 + 18 + 9 + 45));
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (8 + 18 + 45) / (double) (16 + 8 + 9 + 18 + 9 + 45));
    p = {8, 12};
    pFT2->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (18 + 45) / (double) (9 + 18 + 9 + 45));
    pFT1->transform_std_vector(p, v_out);
    BOOST_CHECK_EQUAL(v_out[1], (18 + 45) / (double) (9 + 18 + 9 + 45));
}


BOOST_AUTO_TEST_SUITE_END()
