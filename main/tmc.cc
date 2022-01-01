#include <iostream>

#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../lib/evaluator.h"
#include "../lib/classifier_creator_fs_graph.h"
#include "../lib/classifier_creator_fs_n_fold.h"
#include "../lib/classifier_creator_train_tmc.h"


int main(int ac, char* av[]) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("verbose", "provide detailed info what is going on")
            ("names", boost::program_options::value<std::string>(), "names file")
            ("train-data", boost::program_options::value<std::string>(), "train data file")
            ("eval-data", boost::program_options::value<std::string>(), "evaluation data file")
            ("trained-config", boost::program_options::value<std::string>(), "output file with trained configuration")
            ("ffs-cross-val", "do feature forward selection using N-fold cross validation")
            ("ffs-cross-val-n-folds", boost::program_options::value<size_t>(), "number of folds, default = 2")
            ("ffs-graph", "do feature forward selection using graph analysis")
            ("ffs-graph-pruning-factor", boost::program_options::value<double>(), "pruning factor, default = 1.0, smaller the value more features will be taken")
            ("classify-data", "classify train and evaluation data files - create files with suffix .classified")
            ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    bool verbose = false;
    if ( vm.count("ffs-cross-val") && vm.count("ffs-graph") ) {
       throw std::runtime_error("either --ffs-cross-val or --ffs-graph can be set, not both");
    }

    if (vm.count("verbose")) {
        verbose = true;
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

    std::shared_ptr<ClassifierCreatorTrain> pCC = nullptr;
    if ( vm.count("ffs-cross-val") ) {
        const auto pCCtmc = std::make_shared<ClassifierCreatorTrainTmc>();
        const auto pCCnFold = std::make_shared<ClassifierCreatorFsNfold>();
        pCCnFold->set_classifier_creator_train(pCCtmc);
        size_t n_folds = 2;
        if ( vm.count("ffs-cross-val-n-folds") ) {
            n_folds = vm["ffs-cross-val-n-folds"].as<size_t>();
        }
        pCCnFold->set_n_folds(n_folds);
        pCC = pCCnFold;
    } else if ( vm.count("ffs-graph") ) {
        const auto pCCtmc = std::make_shared<ClassifierCreatorTrainTmc>();
        const auto pCCgraph = std::make_shared<ClassifierCreatorFsGraph>();
        pCCgraph->set_classifier_creator_train(pCCtmc);
        double threshold_br = 1.0;
        if ( vm.count("ffs-graph-pruning-factor") ) {
            threshold_br = vm["ffs-graph-pruning-factor"].as<double>();
        }
        pCCgraph->set_threshold_br(threshold_br);
        pCC = pCCgraph;
    } else {
        pCC = std::make_shared<ClassifierCreatorTrainTmc>();
    }

    pCC->verbose(verbose);
    (*pCC).init(pSampleTrain).train();
    const std::shared_ptr<Classifier> pC = pCC->get_classifier();

    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_classifier(pC).set_conf_type(Evaluator::number);

    (*pEvaluator).set_sample(pSampleTrain);
    const auto confusion_matrix_train = pEvaluator->get_confusion_matrix();
    const auto roc_err_train = pEvaluator->get_roc_error();
    const auto err_rate_train = pEvaluator->get_error_rate();

    (*pEvaluator).set_sample(pSampleEval);
    const auto confusion_matrix_eval = pEvaluator->get_confusion_matrix();
    const auto roc_err_eval = pEvaluator->get_roc_error();
    const auto err_rate_eval = pEvaluator->get_error_rate();

    std::cout << std::endl;
    std::cout << "########################## Performing evaluation ######################################" << std::endl;
    std::cout << "\t\t\t\tTrain conf matr\t###\tEval conf matr" << std::endl;
    std::cout << "predicted pos:\t"
              << confusion_matrix_train.first.first << "\t\t" << confusion_matrix_train.first.second << "\t\t###\t"
              << confusion_matrix_eval.first.first << "\t\t" << confusion_matrix_eval.first.second << std::endl;
    std::cout << "predicted neg:\t"
              << confusion_matrix_train.second.first << "\t\t" << confusion_matrix_train.second.second << "\t\t###\t"
              << confusion_matrix_eval.second.first << "\t\t" << confusion_matrix_eval.second.second << std::endl;
    std::cout << "actually   ->\tpos\t\tneg\t\t###\tpos\t\tneg" << std::endl;
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
        pC->dump_to_ptree(pt);
        boost::property_tree::json_parser::write_json(fileH, pt);
        fileH.close();
        std::cout << "Border system is dumped into file: " << config_file_name << std::endl;
    }

    if (vm.count("classify-data")) {
        std::ofstream fileStream1(train_file + ".classified");
        if (!fileStream1.is_open())
            throw std::runtime_error("Cannot open file " + train_file + ".classified");
        (*pEvaluator).evaluate_data_file(fileStream1,train_file,pFN);

        std::ofstream fileStream2(eval_file + ".classified");
        if (!fileStream2.is_open())
            throw std::runtime_error("Cannot open file " + eval_file + ".classified");
        (*pEvaluator).evaluate_data_file(fileStream2,eval_file,pFN);
    }

    return 0;
}
