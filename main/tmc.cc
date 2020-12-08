#include <iostream>

#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../lib/border_system_creator.h"
#include "../lib/evaluator.h"
#include "../lib/layer_partitioning_creator.h"
#include "../lib/classifier_tmc.h"

int main(int ac, char* av[]) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("names", boost::program_options::value<std::string>(), "names file")
            ("train-data", boost::program_options::value<std::string>(), "train data file")
            ("eval-data", boost::program_options::value<std::string>(), "evaluation data file")
            ("trained-config", boost::program_options::value<std::string>(), "output file with trained configuration");

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
    std::cout << "Training sample loaded: " << pSampleTrain->size() << " unique feature vectors" << std::endl;
    const auto pSampleEval = sample_creator.from_file(eval_file);
    std::cout << "Evaluation sample loaded: " << pSampleEval->size() << " unique feature vectors" << std::endl;

    auto pLayerPartitioningCreator = std::make_shared<LayerPartitioningCreator>();
    pLayerPartitioningCreator->push_back(pSampleTrain);

    std::cout << "Starting tmc optimization... " << std::flush;
    const auto iteration_num = pLayerPartitioningCreator->optimize();
    std::cout << "finished in " << iteration_num << " iterations" << std::endl;
    const auto pLP = pLayerPartitioningCreator->get_layer_partitioning();
    const auto layer_partitioning_size = pLP->size();
    std::cout << "Resulting layer partitioning size: " << layer_partitioning_size << std::endl;

    const auto pBSC = std::make_shared<BorderSystemCreator>();
    const auto pBS = pBSC->from_layer_partitioning(pLP);

    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_classifier(std::make_shared<ClassifierTmc>(pBS)).set_conf_type(Evaluator::number);

    (*pEvaluator).set_sample(pSampleTrain);
    const auto confusion_matrix_train = pEvaluator->get_confusion_matrix();
    const auto roc_err_train = pEvaluator->get_roc_error();
    const auto err_rate_train = pEvaluator->get_error_rate();

    (*pEvaluator).set_sample(pSampleEval);
    const auto confusion_matrix_eval = pEvaluator->get_confusion_matrix();
    const auto roc_err_eval = pEvaluator->get_roc_error();
    const auto err_rate_eval = pEvaluator->get_error_rate();

    std::cout << "########################## Performing evaluation ######################################" << std::endl;
    std::cout << "\t\t\t\tTrain conf matr\t###\tEval conf matr" << std::endl;
    std::cout << "predicted pos:\t"
              << confusion_matrix_train.first.first << "\t\t" << confusion_matrix_train.first.second << "\t\t###\t"
              << confusion_matrix_eval.first.first << "\t\t" << confusion_matrix_eval.first.second << std::endl;
    std::cout << "predicted neg:\t"
              << confusion_matrix_train.second.first << "\t\t" << confusion_matrix_train.second.second << "\t\t###\t"
              << confusion_matrix_eval.second.first << "\t\t" << confusion_matrix_eval.second.second << std::endl;
    std::cout << "actually   ->\tpos\t\tneg\t\t###\tpos\t\t\tneg" << std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\t\t\t\tTrain rank err\t###\tEval rank err" << std::endl;
    std::cout << "\t\t\t\t" << roc_err_train << "\t\t###\t" << roc_err_eval << std::endl;
    std::cout << "---------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\t\t\t\tTrain err rate\t###\tEval err rate" << std::endl;
    std::cout << "\t\t\t\t" << err_rate_train << "\t\t###\t" << err_rate_eval << std::endl;
    std::cout << "#######################################################################################" << std::endl;

    if (vm.count("trained-config")) {
        const auto config_file_name = vm["trained-config"].as<std::string>();
        std::ofstream fileH(config_file_name);
        if (!fileH.is_open())
            throw std::runtime_error("Cannot open file " + config_file_name);
        boost::property_tree::ptree pt;
        pBS->dump_to_ptree(pt);
        boost::property_tree::json_parser::write_json(fileH, pt);
        fileH.close();
        std::cout << "Border system is dumped into file: " << config_file_name << std::endl;
    }

    return 0;
}
