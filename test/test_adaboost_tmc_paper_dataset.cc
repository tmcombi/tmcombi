#define BOOST_TEST_MODULE test_adaboost_tmc_paper_dataset
#include <boost/test/included/unit_test.hpp>

#include "../lib/feature_names.h"
#include "../lib/classifier_creator_adaboost.h"
#include "../lib/classifier_creator_fs_graph.h"
#include "../lib/classifier_creator_fs_n_fold.h"
#include "../lib/classifier_creator_train_tmc.h"

BOOST_AUTO_TEST_CASE( test_adaboost_tmc_1k )
{
    const std::string names_file("data/tmc_paper/tmc_paper.names");
    const std::string data_file("data/tmc_paper/tmc_paper.data");
    const std::string eval_file("data/tmc_paper/tmc_paper.test");
    BOOST_TEST_MESSAGE("Creating train sample from file: " << data_file);
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << eval_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);


    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    std::shared_ptr<Sample> pSampleEval = sample_creator.from_file(eval_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 992);

    std::shared_ptr<ClassifierCreatorTrain> pCC_aux = std::make_shared<ClassifierCreatorTrainTmc>();
    std::shared_ptr<ClassifierCreatorAdaboost> pCCAdaBoost = std::make_shared<ClassifierCreatorAdaboost>();
    pCCAdaBoost->verbose(true);
    (*pCCAdaBoost).set_classifier_creator_train(pCC_aux).
            init(pSample).
            set_eval_sample(pSampleEval).
            set_trials(16).
            train();

    std::shared_ptr<Classifier> pTmcClBoosted = pCCAdaBoost->get_classifier();

    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_conf_type(Evaluator::number).set_classifier(pTmcClBoosted);
    (*pEvaluator).set_sample(pSample);
    BOOST_CHECK_EQUAL(pEvaluator->get_error_rate(), 0.14717741935483872);
    (*pEvaluator).set_sample(pSampleEval);
    BOOST_CHECK_EQUAL(pEvaluator->get_error_rate(), 0.17842741935483872);
}


BOOST_AUTO_TEST_CASE( test_adaboost_fs_graph_tmc_1k )
{
    const std::string names_file("data/tmc_paper/tmc_paper.names");
    const std::string data_file("data/tmc_paper/tmc_paper.data");
    const std::string eval_file("data/tmc_paper/tmc_paper.test");
    BOOST_TEST_MESSAGE("Creating train sample from file: " << data_file);
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << eval_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);


    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    std::shared_ptr<Sample> pSampleEval = sample_creator.from_file(eval_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 992);

    std::shared_ptr<ClassifierCreatorTrain> pCC_aux_tmc = std::make_shared<ClassifierCreatorTrainTmc>();
    std::shared_ptr<ClassifierCreatorFsGraph> pCC_aux = std::make_shared<ClassifierCreatorFsGraph>();
    pCC_aux->set_classifier_creator_train(pCC_aux_tmc);
    pCC_aux->set_threshold_br(0.1);
    pCC_aux->verbose(true);
    std::shared_ptr<ClassifierCreatorAdaboost> pCCAdaBoost = std::make_shared<ClassifierCreatorAdaboost>();
    pCCAdaBoost->verbose(true);
    (*pCCAdaBoost).set_classifier_creator_train(pCC_aux).
            init(pSample).
            set_eval_sample(pSampleEval).
            set_trials(16).
            train();

    std::shared_ptr<Classifier> pTmcClBoosted = pCCAdaBoost->get_classifier();

    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_conf_type(Evaluator::number).set_classifier(pTmcClBoosted);
    (*pEvaluator).set_sample(pSample);
    BOOST_CHECK_EQUAL(pEvaluator->get_error_rate(), 0.14717741935483872);
    (*pEvaluator).set_sample(pSampleEval);
    BOOST_CHECK_EQUAL(pEvaluator->get_error_rate(), 0.17842741935483872);
}

BOOST_AUTO_TEST_CASE( test_adaboost_fs_n_fold_tmc_1k )
{
    const std::string names_file("data/tmc_paper/tmc_paper.names");
    const std::string data_file("data/tmc_paper/tmc_paper.data");
    const std::string eval_file("data/tmc_paper/tmc_paper.test");
    BOOST_TEST_MESSAGE("Creating train sample from file: " << data_file);
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << eval_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);


    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    std::shared_ptr<Sample> pSampleEval = sample_creator.from_file(eval_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 992);

    std::shared_ptr<ClassifierCreatorTrain> pCC_aux_tmc = std::make_shared<ClassifierCreatorTrainTmc>();
    std::shared_ptr<ClassifierCreatorFsNfold> pCC_aux = std::make_shared<ClassifierCreatorFsNfold>();
    pCC_aux->verbose(true);
    (*pCC_aux).set_classifier_creator_train(pCC_aux_tmc).set_kpi_type(ClassifierCreatorFsNfold::class_err);
    std::shared_ptr<ClassifierCreatorAdaboost> pCCAdaBoost = std::make_shared<ClassifierCreatorAdaboost>();
    pCCAdaBoost->verbose(true);
    (*pCCAdaBoost).set_classifier_creator_train(pCC_aux).
            init(pSample).
            set_eval_sample(pSampleEval).
            set_trials(16).
            train();

    std::shared_ptr<Classifier> pTmcClBoosted = pCCAdaBoost->get_classifier();
    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_conf_type(Evaluator::number).set_classifier(pTmcClBoosted);
    (*pEvaluator).set_sample(pSample);
    BOOST_CHECK_EQUAL(pEvaluator->get_error_rate(), 0.17137096774193547);
    (*pEvaluator).set_sample(pSampleEval);
    BOOST_CHECK_EQUAL(pEvaluator->get_error_rate(), 0.19758064516129031);
}

