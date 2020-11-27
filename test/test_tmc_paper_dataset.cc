#define BOOST_TEST_MODULE test_tmc_paper_dataset
#include <boost/test/included/unit_test.hpp>

//#define TIMERS
//#define TRACE_EVALUATOR

#include "../lib/evaluator.h"
#include "../lib/border_system_creator.h"
#include "../lib/layer_partitioning_creator.h"


BOOST_AUTO_TEST_CASE( tmc_paper_dataset_do_one_step ) {
    const std::string names_file("data/tmc_paper/tmc_paper.names");
    const std::string data_file("data/tmc_paper/tmc_paper.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 1);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
    pLayerPartitioningCreator->do_one_step();
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 2);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
    pLayerPartitioningCreator->do_one_step();
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 3);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
    pLayerPartitioningCreator->do_one_step();
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 4);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);
}

BOOST_AUTO_TEST_CASE( tmc_paper_dataset_optimize ) {
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
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);

    pLayerPartitioningCreator->optimize();
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->size(), 29);
    BOOST_CHECK_EQUAL(pLayerPartitioningCreator->get_layer_partitioning()->consistent(), true);

    auto pEvaluator = std::make_shared<Evaluator>();
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;

    const auto pLP = pLayerPartitioningCreator->get_layer_partitioning();
    const auto pBSC = std::make_shared<BorderSystemCreator>();
    const auto pBS = pBSC->from_layer_partitioning(pLP);
    (*pEvaluator).set_border_system(pBS);

    (*pEvaluator).set_sample(pSample);
    confusion_matrix_train = pEvaluator->get_confusion_matrix();
    roc_err_train = pEvaluator->get_roc_error();
    err_rate_train = pEvaluator->get_error_rate();

    (*pEvaluator).set_sample(pSampleEval);
    confusion_matrix_eval = pEvaluator->get_confusion_matrix();
    roc_err_eval = pEvaluator->get_roc_error();
    err_rate_eval = pEvaluator->get_error_rate();

    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("################################################");
    BOOST_TEST_MESSAGE("Train conf matr ### Eval conf matr");
    BOOST_TEST_MESSAGE(confusion_matrix_train.first.first << "  " << confusion_matrix_train.first.second << "    ###    "
                                                          << confusion_matrix_eval.first.first << "   " << confusion_matrix_eval.first.second);
    BOOST_TEST_MESSAGE(confusion_matrix_train.second.first << "  " << confusion_matrix_train.second.second << "    ###    "
                                                           << confusion_matrix_eval.second.first << "   " << confusion_matrix_eval.second.second);
    BOOST_TEST_MESSAGE("------------------------------------------------");
    BOOST_TEST_MESSAGE("Train rank err ### Eval rank err");
    BOOST_TEST_MESSAGE( roc_err_train << "    ###    " << roc_err_eval);
    BOOST_TEST_MESSAGE("------------------------------------------------");
    BOOST_TEST_MESSAGE("Train err rate ### Eval err rate");
    BOOST_TEST_MESSAGE( err_rate_train << "    ###    " << err_rate_eval);
    BOOST_TEST_MESSAGE("################################################");

    BOOST_CHECK_EQUAL(roc_err_train, 0.070182427159209151);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.11113911290322581);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{415,65},{81,431}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{391,79.5},{105,416.5}});
    BOOST_CHECK(confusion_matrix_eval == confusion_matrix_eval_);
}

BOOST_AUTO_TEST_CASE( tmc_paper_dataset_evaluation ) {
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
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);
    auto pEvaluator = std::make_shared<Evaluator>();
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;
    unsigned int counter = 0;
    do {
        const auto pLP = pLayerPartitioningCreator->get_layer_partitioning();
        const auto pBSC = std::make_shared<BorderSystemCreator>();
        const auto pBS = pBSC->from_layer_partitioning(pLP);
        (*pEvaluator).set_border_system(pBS);

        (*pEvaluator).set_sample(pSample);
        confusion_matrix_train = pEvaluator->get_confusion_matrix();
        roc_err_train = pEvaluator->get_roc_error();
        err_rate_train = pEvaluator->get_error_rate();

        (*pEvaluator).set_sample(pSampleEval);
        confusion_matrix_eval = pEvaluator->get_confusion_matrix();
        roc_err_eval = pEvaluator->get_roc_error();
        err_rate_eval = pEvaluator->get_error_rate();

        BOOST_TEST_MESSAGE("");
        BOOST_TEST_MESSAGE("############      Iteration " << counter++ << "       ############");
        BOOST_TEST_MESSAGE("Train conf matr ### Eval conf matr");
        BOOST_TEST_MESSAGE(confusion_matrix_train.first.first << "  " << confusion_matrix_train.first.second << "    ###    "
                                                              << confusion_matrix_eval.first.first << "   " << confusion_matrix_eval.first.second);
        BOOST_TEST_MESSAGE(confusion_matrix_train.second.first << "  " << confusion_matrix_train.second.second << "    ###    "
                                                               << confusion_matrix_eval.second.first << "   " << confusion_matrix_eval.second.second);
        BOOST_TEST_MESSAGE("------------------------------------------------");
        BOOST_TEST_MESSAGE("Train rank err ### Eval rank err");
        BOOST_TEST_MESSAGE( roc_err_train << "    ###    " << roc_err_eval);
        BOOST_TEST_MESSAGE("------------------------------------------------");
        BOOST_TEST_MESSAGE("Train err rate ### Eval err rate");
        BOOST_TEST_MESSAGE( err_rate_train << "    ###    " << err_rate_eval);
        BOOST_TEST_MESSAGE("################################################");

    }
    while (pLayerPartitioningCreator->do_one_step());
    BOOST_CHECK_EQUAL(roc_err_train, 0.070182427159209151);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.11113911290322581);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{415,65},{81,431}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{391,79.5},{105,416.5}});
    BOOST_CHECK(confusion_matrix_eval == confusion_matrix_eval_);
}

BOOST_AUTO_TEST_CASE( tmc_paper_dataset_evaluation_sample_split ) {
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

    BOOST_TEST_MESSAGE("Resulting sample size: " << pSample->size());
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 992);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 992);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    //pLayerPartitioningCreator->push_back(pSample);
    pLayerPartitioningCreator->push_back_with_sample_split(pSample,992/2);
    auto pEvaluator = std::make_shared<Evaluator>();
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;
    unsigned int counter = 0;
    do {
        const auto pLP = pLayerPartitioningCreator->get_layer_partitioning();
        const auto pBSC = std::make_shared<BorderSystemCreator>();
        const auto pBS = pBSC->from_layer_partitioning(pLP);
        (*pEvaluator).set_border_system(pBS);

        (*pEvaluator).set_sample(pSample);
        confusion_matrix_train = pEvaluator->get_confusion_matrix();
        roc_err_train = pEvaluator->get_roc_error();
        err_rate_train = pEvaluator->get_error_rate();

        (*pEvaluator).set_sample(pSampleEval);
        confusion_matrix_eval = pEvaluator->get_confusion_matrix();
        roc_err_eval = pEvaluator->get_roc_error();
        err_rate_eval = pEvaluator->get_error_rate();

        BOOST_TEST_MESSAGE("");
        BOOST_TEST_MESSAGE("############      Iteration " << counter++ << "       ############");
        BOOST_TEST_MESSAGE("Train conf matr ### Eval conf matr");
        BOOST_TEST_MESSAGE(confusion_matrix_train.first.first << "  " << confusion_matrix_train.first.second << "    ###    "
                                                              << confusion_matrix_eval.first.first << "   " << confusion_matrix_eval.first.second);
        BOOST_TEST_MESSAGE(confusion_matrix_train.second.first << "  " << confusion_matrix_train.second.second << "    ###    "
                                                               << confusion_matrix_eval.second.first << "   " << confusion_matrix_eval.second.second);
        BOOST_TEST_MESSAGE("------------------------------------------------");
        BOOST_TEST_MESSAGE("Train rank err ### Eval rank err");
        BOOST_TEST_MESSAGE( roc_err_train << "    ###    " << roc_err_eval);
        BOOST_TEST_MESSAGE("------------------------------------------------");
        BOOST_TEST_MESSAGE("Train err rate ### Eval err rate");
        BOOST_TEST_MESSAGE( err_rate_train << "    ###    " << err_rate_eval);
        BOOST_TEST_MESSAGE("################################################");

    }
    while (pLayerPartitioningCreator->do_one_step());
    BOOST_CHECK_EQUAL(roc_err_train, 0.070182427159209151);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.11113911290322581);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{415,65},{81,431}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{391,79.5},{105,416.5}});
    BOOST_CHECK(confusion_matrix_eval == confusion_matrix_eval_);
}

