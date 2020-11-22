#define BOOST_TEST_MODULE test_fs_adult_transformed_dataset
#include <boost/test/included/unit_test.hpp>

//#define TIMERS
//#define TRACE_EVALUATOR

#include "../lib/forward_selection.h"



BOOST_AUTO_TEST_CASE( fs_adult_transformed_dataset_roc_err  ) {
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
    std::shared_ptr<ForwardSelection> pFS = std::make_shared<ForwardSelection>();
    pFS->set_sample(pSample);
    pFS->init();

    boost::dynamic_bitset<> active_features, active_features_sign;
    std::tie(active_features,active_features_sign) = pFS->optimize();


    ////////////////////////////////////////////////////
    ////////   create FS-reduced samples        ////////
    ////////////////////////////////////////////////////
    std::vector<unsigned int> feature_indices;
    std::vector<bool> signs;
    for (unsigned int i=0; i < active_features.size(); i++) {
        if (active_features[i]) {
            feature_indices.push_back(i);
            signs.push_back(active_features_sign[i]);
        }
    }
    std::shared_ptr<Sample> pSampleFS = std::make_shared<Sample>(feature_indices.size());
    std::shared_ptr<Sample> pSampleEvalFS = std::make_shared<Sample>(feature_indices.size());
    for (unsigned int i=0; i < pSample->size(); i++) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                (*(*pSample)[i], feature_indices, signs);
        pSampleFS->push(pFV);
    }

    for (unsigned int i=0; i < pSampleEval->size(); i++) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                (*(*pSampleEval)[i], feature_indices, signs);
        pSampleEvalFS->push(pFV);
    }
    ////////////////////////////////////////////////////

    BOOST_CHECK_EQUAL(pSampleFS->dim(), 7);
    BOOST_CHECK_EQUAL(pSampleFS->size(), 4926);
    BOOST_CHECK_EQUAL(pSampleEvalFS->size(), 3245);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSampleFS);
    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_conf_type(Evaluator::number);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;
    unsigned int counter = 0;
    do {
        BOOST_TEST_MESSAGE("############      Iteration " << counter++ << "       ############");
    }
    while (pLayerPartitioningCreator->do_one_step());
    const auto pLP = pLayerPartitioningCreator->get_layer_partitioning();
    const auto pBSC = std::make_shared<BorderSystemCreator>();
    const auto pBS = pBSC->from_layer_partitioning(pLP);
    (*pEvaluator).set_border_system(pBS);

    (*pEvaluator).set_sample(pSampleFS);
    confusion_matrix_train = pEvaluator->get_confusion_matrix();
    roc_err_train = pEvaluator->get_roc_error();
    err_rate_train = pEvaluator->get_error_rate();

    (*pEvaluator).set_sample(pSampleEvalFS);
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
    BOOST_CHECK_EQUAL(roc_err_train, 0.083333914244791085);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.094753796801522211);
    BOOST_CHECK_EQUAL(err_rate_train, 0.14209865927551998);
    BOOST_CHECK_EQUAL(err_rate_eval, 0.14730395866297366);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{880440398,281263013},{532189518,4030668690}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{423342968,145052970},{275591534,2011635064}});
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

BOOST_AUTO_TEST_CASE( fs_adult_transformed_dataset_err_rate  ) {
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
    std::shared_ptr<ForwardSelection> pFS = std::make_shared<ForwardSelection>();
    pFS->set_sample(pSample);
    pFS->set_objective_kpi_type(ForwardSelection::err_rate);
    pFS->init();
    boost::dynamic_bitset<> active_features, active_features_sign;
    std::tie(active_features,active_features_sign) = pFS->optimize();

    ////////////////////////////////////////////////////
    ////////   create FS-reduced samples        ////////
    ////////////////////////////////////////////////////
    std::vector<unsigned int> feature_indices;
    std::vector<bool> signs;
    for (unsigned int i=0; i < active_features.size(); i++) {
        if (active_features[i]) {
            feature_indices.push_back(i);
            signs.push_back(active_features_sign[i]);
        }
    }
    std::shared_ptr<Sample> pSampleFS = std::make_shared<Sample>(feature_indices.size());
    std::shared_ptr<Sample> pSampleEvalFS = std::make_shared<Sample>(feature_indices.size());
    for (unsigned int i=0; i < pSample->size(); i++) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                (*(*pSample)[i], feature_indices, signs);
        pSampleFS->push(pFV);
    }

    for (unsigned int i=0; i < pSampleEval->size(); i++) {
        std::shared_ptr<FeatureVector> pFV = std::make_shared<FeatureVector>
                (*(*pSampleEval)[i], feature_indices, signs);
        pSampleEvalFS->push(pFV);
    }
    ///////////////////////////////////////////////////

    BOOST_CHECK_EQUAL(pSampleFS->dim(), 6);
    BOOST_CHECK_EQUAL(pSampleFS->size(), 4869);
    BOOST_CHECK_EQUAL(pSampleEvalFS->size(), 2995);
    std::shared_ptr<LayerPartitioningCreator> pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSampleFS);
    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_conf_type(Evaluator::number);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train, confusion_matrix_eval;
    double roc_err_train, roc_err_eval, err_rate_train, err_rate_eval;
    unsigned int counter = 0;
    do {
        BOOST_TEST_MESSAGE("############      Iteration " << counter++ << "       ############");
    }
    while (pLayerPartitioningCreator->do_one_step());
    const auto pLP = pLayerPartitioningCreator->get_layer_partitioning();
    const auto pBSC = std::make_shared<BorderSystemCreator>();
    const auto pBS = pBSC->from_layer_partitioning(pLP);
    (*pEvaluator).set_border_system(pBS);

    (*pEvaluator).set_sample(pSampleFS);
    confusion_matrix_train = pEvaluator->get_confusion_matrix();
    roc_err_train = pEvaluator->get_roc_error();
    err_rate_train = pEvaluator->get_error_rate();

    (*pEvaluator).set_sample(pSampleEvalFS);
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
    BOOST_CHECK_EQUAL(roc_err_train, 0.085877164784375348);
    BOOST_CHECK_EQUAL(roc_err_eval, 0.096041612833839246);
    BOOST_CHECK_EQUAL(err_rate_train, 0.14130001419065868);
    BOOST_CHECK_EQUAL(err_rate_eval, 0.14615821689957414);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_train_({{841937893,238188615},{570692023,4073743088}});
    BOOST_CHECK(confusion_matrix_train == confusion_matrix_train_);
    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix_eval_({{405845997,124284193},{293088505,2032403841}});
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
