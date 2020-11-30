#include <iostream>

#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../lib/border_system.h"

void print(const std::vector<double> & v, double conf, const std::string & label) {
    std::cout << "Vector {" << v[0] << "," << v[1] << "} from evaluation data sample has conf = " << conf
    << " (compared to 0.5) and therefore is classified as \"" << label << "\"" << std::endl;
}

std::string label(double conf) {
    if (conf < 0.5) {
        return "0";
    }
    if (conf > 0.5) {
        return "1";
    }
    return "undefined";
}

int main(int ac, char* av[]) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("trained-config", boost::program_options::value<std::string>(), "output file with trained configuration");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    if (vm.count("trained-config")) {
        std::cout << "Trained configuration file was set to "
                  << vm["trained-config"].as<std::string>() << std::endl;
    } else {
        std::cout << "Please set trained configuration file" << std::endl;
        return 0;
    }
    const auto config_file_name = vm["trained-config"].as<std::string>();

    std::ifstream fileH(config_file_name);
    if (!fileH.is_open())
        throw std::runtime_error("Cannot open file " + config_file_name);
    std::cout << "Loading boder system from file: " << config_file_name << std::endl;
    boost::property_tree::ptree pt;
    boost::property_tree::json_parser::read_json(fileH, pt);

    const auto pBS = std::make_shared<BorderSystem>(pt);

    std::cout << "Classifying two first vectors of positives and negatives from the evaluation data sample";
    std::cout << std::endl;

    const std::vector<double> p_v1({0.996323,0.805821});
    const std::vector<double> p_v2({0.452852,0.291811});

    const std::vector<double> n_v1({0.187739,0.031114});
    const std::vector<double> n_v2({0.046231,0.153799});

    const double p_v1_conf = pBS->confidence(p_v1); const std::string p_v1_label = label(p_v1_conf);
    const double p_v2_conf = pBS->confidence(p_v2); const std::string p_v2_label = label(p_v2_conf);

    const double n_v1_conf = pBS->confidence(n_v1); const std::string n_v1_label = label(n_v1_conf);
    const double n_v2_conf = pBS->confidence(n_v2); const std::string n_v2_label = label(n_v2_conf);

    print(p_v1, p_v1_conf, p_v1_label);
    print(p_v2, p_v2_conf, p_v2_label);
    print(n_v1, n_v1_conf, n_v1_label);
    print(n_v2, n_v2_conf, n_v2_label);

    return 0;
}
