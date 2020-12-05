#define BOOST_TEST_MODULE test_adult_transformed_dataset
#include <boost/test/included/unit_test.hpp>

#define TIMERS
//#define TRACE_EVALUATOR

#include "../lib/evaluator.h"
#include "../lib/border_system_creator.h"
#include "../lib/layer_partitioning_creator.h"

BOOST_AUTO_TEST_CASE( adult_transformed_dataset_2_5k ) {
    const std::string names_file("data/adult_transformed/adult_transformed.names");
    const std::string data_file("data/adult_transformed/adult_transformed_2.5k.data");
    const std::string eval_file("data/adult_transformed/adult_transformed.test");
    BOOST_TEST_MESSAGE("Creating train sample from file: " << data_file);
    BOOST_TEST_MESSAGE("Creating evaluation sample from file: " << eval_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    std::shared_ptr<Sample> pSampleEval = sample_creator.from_file(eval_file);

    BOOST_TEST_MESSAGE("Resulting sample size: " << pSample->size());
    BOOST_CHECK_EQUAL(pSample->dim(), 13);
    BOOST_CHECK_EQUAL(pSample->size(), 2465);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 13906);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);
    auto pEvaluator = std::make_shared<Evaluator>();
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;
    size_t counter = 0;
    do {
        BOOST_TEST_MESSAGE("############      Iteration " << counter++ << "       ############");
    }
    while (pLayerPartitioningCreator->do_one_step());
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
        BOOST_TEST_MESSAGE((long int)confusion_matrix_train.first.first << "  " << (long int)confusion_matrix_train.first.second << "    ###    "
                                                                        << (long int)confusion_matrix_eval.first.first << "   " << (long int)confusion_matrix_eval.first.second);
        BOOST_TEST_MESSAGE((long int)confusion_matrix_train.second.first << "  " << (long int)confusion_matrix_train.second.second << "    ###    "
                                                                         << (long int)confusion_matrix_eval.second.first << "   " << (long int)confusion_matrix_eval.second.second);
        BOOST_TEST_MESSAGE("------------------------------------------------");
        BOOST_TEST_MESSAGE("Train rank err ### Eval rank err");
        BOOST_TEST_MESSAGE( roc_err_train << "    ###    " << roc_err_eval);
        BOOST_TEST_MESSAGE("------------------------------------------------");
        BOOST_TEST_MESSAGE("Train err rate ### Eval err rate");
        BOOST_TEST_MESSAGE( err_rate_train << "    ###    " << err_rate_eval);
        BOOST_TEST_MESSAGE("################################################");
//}
//while (pLayerPartitioningCreator->do_one_step());
    BOOST_CHECK_EQUAL(roc_err_train, 0.023456166910557547);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.18955266653760158);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{98637823,15610269},{22187662,338645086}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{421210579.5,274297694.5},{277723922.5,1882390339.5}});
    BOOST_CHECK(confusion_matrix_eval == confusion_matrix_eval_);
}

BOOST_AUTO_TEST_CASE( adult_transformed_dataset ) {
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

    BOOST_TEST_MESSAGE("Resulting sample size: " << pSample->size());
    BOOST_CHECK_EQUAL(pSample->dim(), 13);
    BOOST_CHECK_EQUAL(pSample->size(), 26314);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 13906);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSample);
    auto pEvaluator = std::make_shared<Evaluator>();
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;
    size_t counter = 0;
    do {
        BOOST_TEST_MESSAGE("############      Iteration " << counter++ << "       ############");
    }
    while (pLayerPartitioningCreator->do_one_step());
    const auto pLP = pLayerPartitioningCreator->get_layer_partitioning();
    const auto pBSC = std::make_shared<BorderSystemCreator>();
    const auto pBS = pBSC->from_layer_partitioning(pLP);
    (*pEvaluator).set_border_system(pBS);

    (*pEvaluator).set_sample(pSample);
    confusion_matrix_train = pEvaluator->get_confusion_matrix();
    roc_err_train = pEvaluator->get_roc_error();
    err_rate_train = pEvaluator->get_error_rate();
    /*
    std::ofstream fs;
    fs.open(data_file+".eval");
    if (!fs.is_open())
        throw std::runtime_error("Cannot open file: " + data_file + ".eval");
    pEvaluator->evaluate_data_file(fs, data_file, pFN);
    fs.close();
     */

    (*pEvaluator).set_sample(pSampleEval);
    confusion_matrix_eval = pEvaluator->get_confusion_matrix();
    roc_err_eval = pEvaluator->get_roc_error();
    err_rate_eval = pEvaluator->get_error_rate();
    /*
    fs.open(eval_file+".eval");
    if (!fs.is_open())
        throw std::runtime_error("Cannot open file: " + eval_file + ".eval");
    pEvaluator->evaluate_data_file(fs, eval_file, pFN);
    fs.close();
     */

    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("############      Iteration " << counter++ << "       ############");
    BOOST_TEST_MESSAGE("Train conf matr ### Eval conf matr");
    BOOST_TEST_MESSAGE((long int)confusion_matrix_train.first.first << "  " << (long int)confusion_matrix_train.first.second << "    ###    "
                                                                    << (long int)confusion_matrix_eval.first.first << "   " << (long int)confusion_matrix_eval.first.second);
    BOOST_TEST_MESSAGE((long int)confusion_matrix_train.second.first << "  " << (long int)confusion_matrix_train.second.second << "    ###    "
                                                                     << (long int)confusion_matrix_eval.second.first << "   " << (long int)confusion_matrix_eval.second.second);
    BOOST_TEST_MESSAGE("------------------------------------------------");
    BOOST_TEST_MESSAGE("Train rank err ### Eval rank err");
    BOOST_TEST_MESSAGE( roc_err_train << "    ###    " << roc_err_eval);
    BOOST_TEST_MESSAGE("------------------------------------------------");
    BOOST_TEST_MESSAGE("Train err rate ### Eval err rate");
    BOOST_TEST_MESSAGE( err_rate_train << "    ###    " << err_rate_eval);
    BOOST_TEST_MESSAGE("################################################");
//}
//while (pLayerPartitioningCreator->do_one_step());
    BOOST_CHECK_EQUAL(roc_err_train, 0.04449670928309904);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.12003629225644707);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{1067540668.5,274333677.5},{345089247.5,4037598025.5}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{464481958.5,232380654.5},{234452543.5,1924307379.5}});
    BOOST_CHECK(confusion_matrix_eval == confusion_matrix_eval_);
}

BOOST_AUTO_TEST_CASE( adult_transformed_dataset_sample_split ) {
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

    BOOST_TEST_MESSAGE("Resulting sample size: " << pSample->size());
    BOOST_CHECK_EQUAL(pSample->dim(), 13);
    BOOST_CHECK_EQUAL(pSample->size(), 26314);
    BOOST_CHECK_EQUAL(pSampleEval->size(), 13906);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    //pLayerPartitioningCreator->push_back(pSample);
    pLayerPartitioningCreator->push_back_with_sample_split(pSample,5000);
    auto pEvaluator = std::make_shared<Evaluator>();
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;
    size_t counter = 0;
    do {
        BOOST_TEST_MESSAGE("############      Iteration " << counter++ << "       ############");
    }
    while (pLayerPartitioningCreator->do_one_step());
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
    BOOST_TEST_MESSAGE((long int)confusion_matrix_train.first.first << "  " << (long int)confusion_matrix_train.first.second << "    ###    "
                                                                    << (long int)confusion_matrix_eval.first.first << "   " << (long int)confusion_matrix_eval.first.second);
    BOOST_TEST_MESSAGE((long int)confusion_matrix_train.second.first << "  " << (long int)confusion_matrix_train.second.second << "    ###    "
                                                                     << (long int)confusion_matrix_eval.second.first << "   " << (long int)confusion_matrix_eval.second.second);
    BOOST_TEST_MESSAGE("------------------------------------------------");
    BOOST_TEST_MESSAGE("Train rank err ### Eval rank err");
    BOOST_TEST_MESSAGE( roc_err_train << "    ###    " << roc_err_eval);
    BOOST_TEST_MESSAGE("------------------------------------------------");
    BOOST_TEST_MESSAGE("Train err rate ### Eval err rate");
    BOOST_TEST_MESSAGE( err_rate_train << "    ###    " << err_rate_eval);
    BOOST_TEST_MESSAGE("################################################");
//}
//while (pLayerPartitioningCreator->do_one_step());
    BOOST_CHECK_EQUAL(roc_err_train, 0.04449670928309904);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.12003629225644707);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{1067540668.5,274333677.5},{345089247.5,4037598025.5}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{464481958.5,232380654.5},{234452543.5,1924307379.5}});
    BOOST_CHECK(confusion_matrix_eval == confusion_matrix_eval_);
}

