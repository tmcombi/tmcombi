#define BOOST_TEST_MODULE test_fs_n_fold_adult_transformed_dataset
#include <boost/test/included/unit_test.hpp>

//#define TIMERS
//#define TRACE_EVALUATOR

#include "../lib/classifier_creator_fs_n_fold.h"
#include "../lib/classifier_creator_train_tmc.h"


BOOST_AUTO_TEST_CASE( test_fs_n_fold_adult_transformed_dataset_roc  ) {
    const std::string names_file("data/adult_transformed/adult_transformed.names");
    const std::string data_file("data/adult_transformed/adult_transformed.data");
    const std::string eval_file("data/adult_transformed/adult_transformed.test");
    BOOST_TEST_MESSAGE("Creating train sample from file: " << data_file);
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << eval_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    std::shared_ptr<Sample> pSampleEval = sample_creator.from_file(eval_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 13);
    BOOST_CHECK_EQUAL(pSample->size(), 26314);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 13906);
    std::shared_ptr<ClassifierCreatorTrain> pTC_aux = std::make_shared<ClassifierCreatorTrainTmc>();
    std::shared_ptr<ClassifierCreatorFsNfold> pCCFS = std::make_shared<ClassifierCreatorFsNfold>();
    pCCFS->verbose(true);
    (*pCCFS).set_classifier_creator_train(pTC_aux).
            init(pSample).
            set_n_folds(5).
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

    BOOST_CHECK_EQUAL(roc_err_train, 0.074392899680209762);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.09294518600907764);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{908866308,265218900},{503763608,4046712803}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{427794236,152200867},{271140266,2004487167}});
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

BOOST_AUTO_TEST_CASE( test_fs_n_fold_adult_transformed_dataset_classerr  ) {
    const std::string names_file("data/adult_transformed/adult_transformed.names");
    const std::string data_file("data/adult_transformed/adult_transformed.data");
    const std::string eval_file("data/adult_transformed/adult_transformed.test");
    BOOST_TEST_MESSAGE("Creating train sample from file: " << data_file);
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << eval_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    std::shared_ptr<Sample> pSampleEval = sample_creator.from_file(eval_file);

    BOOST_CHECK_EQUAL(pSample->dim(), 13);
    BOOST_CHECK_EQUAL(pSample->size(), 26314);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 13906);
    std::shared_ptr<ClassifierCreatorTrain> pTC_aux = std::make_shared<ClassifierCreatorTrainTmc>();
    std::shared_ptr<ClassifierCreatorFsNfold> pCCFS = std::make_shared<ClassifierCreatorFsNfold>();
    pCCFS->verbose(true);
    (*pCCFS).set_classifier_creator_train(pTC_aux).
            init(pSample).
            set_n_folds(2).
            set_kpi_type(ClassifierCreatorFsNfold::class_err).
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

    BOOST_CHECK_EQUAL(roc_err_train, 0.089554665836618294);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.095041575784745022);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{822729363,233218553},{589900553,4078713150}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{401599020,118273444},{297335482,2038414590}});
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
