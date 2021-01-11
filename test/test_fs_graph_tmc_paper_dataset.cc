#define BOOST_TEST_MODULE test_fs_graph_tmc_paper_dataset
#include <boost/test/included/unit_test.hpp>

//#define TIMERS
//#define TRACE_EVALUATOR

#include "../lib/classifier_creator_fs_graph.h"
#include "../lib/classifier_creator_train_tmc.h"
#include "../lib/evaluator.h"


BOOST_AUTO_TEST_CASE( test_fs_graph_tmc_paper_dataset  ) {
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
    std::shared_ptr<ClassifierCreatorTrain> pTC_aux = std::make_shared<ClassifierCreatorTrainTmc>();
    std::shared_ptr<ClassifierCreatorFsGraph> pCCFS = std::make_shared<ClassifierCreatorFsGraph>();
    pCCFS->verbose(true);
    (*pCCFS).set_classifier_creator_train(pTC_aux).
            init(pSample).
            set_threshold_br(1).
            train();

    std::shared_ptr<Classifier> pFsClTmc = pCCFS->get_classifier();

    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_conf_type(Evaluator::number).set_classifier(pFsClTmc);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;

        (*pEvaluator).set_sample(pSample);
        confusion_matrix_train = pEvaluator->get_confusion_matrix();
        roc_err_train = pEvaluator->get_roc_error();
        err_rate_train = pEvaluator->get_error_rate();

        (*pEvaluator).set_sample(pSampleEval);
        confusion_matrix_eval = pEvaluator->get_confusion_matrix();
        roc_err_eval = pEvaluator->get_roc_error();
        err_rate_eval = pEvaluator->get_error_rate();

        BOOST_TEST_MESSAGE("");
        BOOST_TEST_MESSAGE("########################");
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

    BOOST_CHECK_EQUAL(roc_err_train, 0.092142380983350675);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.11003146136836628);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{428,102},{68,394}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{405.5,106},{90.5,390}});
    BOOST_CHECK(confusion_matrix_eval == confusion_matrix_eval_);


    BOOST_CHECK_EQUAL(confusion_matrix_train.first.first,confusion_matrix_train_.first.first);
    BOOST_CHECK_EQUAL(confusion_matrix_train.first.second,confusion_matrix_train_.first.second);
    BOOST_CHECK_EQUAL(confusion_matrix_train.second.first,confusion_matrix_train_.second.first);
    BOOST_CHECK_EQUAL(confusion_matrix_train.second.second,confusion_matrix_train_.second.second);

    BOOST_CHECK_EQUAL(confusion_matrix_eval.first.first,confusion_matrix_eval_.first.first);
    BOOST_CHECK_EQUAL(confusion_matrix_eval.first.second,confusion_matrix_eval_.first.second);
    BOOST_CHECK_EQUAL(confusion_matrix_eval.second.first,confusion_matrix_eval_.second.first);
    BOOST_CHECK_EQUAL(confusion_matrix_eval.second.second,confusion_matrix_eval_.second.second);
}
