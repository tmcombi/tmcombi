#include <boost/program_options.hpp>

#include <iostream>
#include "../lib/border_system_creator.h"
#include "../lib/evaluator.h"
#include "../lib/layer_partitioning_creator.h"


int main(int ac, char* av[]) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("verbose", boost::program_options::value<std::string>(), "verbose")
            ("names", boost::program_options::value<std::string>(), "names file")
            ("train-data", boost::program_options::value<std::string>(), "train data file")
            ("eval-data", boost::program_options::value<std::string>(), "evaluation data file");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    if (vm.count("names")) {
        std::cout << "Names file was set to "
                  << vm["names"].as<std::string>() << std::endl;
    } else {
        std::cout << "Please set names file" << std::endl;
        return 0;
    }
    if (vm.count("train-data")) {
        std::cout << "Train data file was set to "
                  << vm["train-data"].as<std::string>() << std::endl;
    } else {
        std::cout << "Please set train data file" << std::endl;
        return 0;
    }
    if (vm.count("eval-data")) {
        std::cout << "Evaluation data file was set to "
                  << vm["eval-data"].as<std::string>() << std::endl;
    } else {
        std::cout << "Please set evaluation data file" << std::endl;
        return 0;
    }

    const std::string names_file = vm["names"].as<std::string>();
    const std::string train_file = vm["train-data"].as<std::string>();
    const std::string eval_file = vm["eval-data"].as<std::string>();

    const auto pFN = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    const auto pSampleTrain = sample_creator.from_file(train_file);
    const auto pSampleEval = sample_creator.from_file(eval_file);

    auto pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSampleTrain);

    const auto iteration_num = pLayerPartitioningCreator->optimize();
    const auto pLP = pLayerPartitioningCreator->get_layer_partitioning();
    const auto layer_partitioning_size = pLP->size();

    const auto pBSC = std::make_shared<BorderSystemCreator>();
    const auto pBS = pBSC->from_layer_partitioning(pLP);

    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_border_system(pBS);

    (*pEvaluator).set_sample(pSampleTrain);
    const auto confusion_matrix_train = pEvaluator->get_confusion_matrix();
    const auto roc_err_train = pEvaluator->get_roc_error();
    const auto err_rate_train = pEvaluator->get_error_rate();

    (*pEvaluator).set_sample(pSampleEval);
    const auto confusion_matrix_eval = pEvaluator->get_confusion_matrix();
    const auto roc_err_eval = pEvaluator->get_roc_error();
    const auto err_rate_eval = pEvaluator->get_error_rate();

    std::cout << "################################################" << std::endl;
    std::cout << "Train conf matr ### Eval conf matr" << std::endl;
    std::cout << confusion_matrix_train.first.first << "  " << confusion_matrix_train.first.second << "    ###    "
              << confusion_matrix_eval.first.first << "   " << confusion_matrix_eval.first.second << std::endl;
    std::cout << confusion_matrix_train.second.first << "  " << confusion_matrix_train.second.second << "    ###    "
              << confusion_matrix_eval.second.first << "   " << confusion_matrix_eval.second.second << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Train rank err ### Eval rank err" << std::endl;
    std::cout << roc_err_train << "    ###    " << roc_err_eval << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Train err rate ### Eval err rate" << std::endl;
    std::cout << err_rate_train << "    ###    " << err_rate_eval << std::endl;
    std::cout << "################################################" << std::endl;

    return 0;
}
