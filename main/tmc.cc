#include <boost/program_options.hpp>

#include <iostream>

int main(int ac, char* av[]) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
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
                  << vm["names"].as<std::string>() << ".\n";
    } else {
        std::cout << "Please set names file\n";
        return 0;
    }

    if (vm.count("train-data")) {
        std::cout << "Train data file was set to "
                  << vm["train-data"].as<std::string>() << ".\n";
    } else {
        std::cout << "Please set train data file\n";
        return 0;
    }

    if (vm.count("eval-data")) {
        std::cout << "Evaluation data file was set to "
                  << vm["eval-data"].as<std::string>() << ".\n";
    } else {
        std::cout << "Please set evaluation data file\n";
        return 0;
    }

    return 0;
}
