#ifndef LIB_FEATURE_NAMES_H_
#define LIB_FEATURE_NAMES_H_

#include <regex>
#include <boost/algorithm/string.hpp>

class FeatureNames {
public:
    explicit FeatureNames(std::istream &);
    explicit FeatureNames(const std::string &);
    ~FeatureNames();

    unsigned int get_dim() const ;

    const std::vector<std::string> & get_feature_names() const;
    const std::vector<unsigned int> & get_feature_indices() const;

    const std::string & get_target_feature_name() const;
    unsigned int get_target_feature_index() const;

    const std::string & get_negatives_label() const;
    const std::string & get_positives_label() const;

    int get_weight_index() const;

private:
    std::vector<std::string> feature_names_;
    std::vector<unsigned int> feature_indices_;
    std::string target_feature_name_;
    unsigned int target_feature_index_{};
    std::string negatives_label_;
    std::string positives_label_;
    int weight_index_{};

    void init_from_stream(std::istream &);
};

void FeatureNames::init_from_stream(std::istream & is)
{
weight_index_ = -1;
    unsigned int index = 0;
    bool target_feature_found = false;
    std::string line;
    while (std::getline(is, line))
    {
        std::string line1 = std::regex_replace (line,std::regex("\r$"),"");
        line1 = std::regex_replace (line1,std::regex("\\|.*"),"");
        line1 = std::regex_replace (line1,std::regex(" *$"),"");
        line1 = std::regex_replace (line1,std::regex("^ *"),"");
        line1 = std::regex_replace (line1,std::regex("\\.$"),"");
        if (line1.empty()) continue;
        std::vector<std::string> line1_vector;
        boost::split(line1_vector, line1, boost::is_any_of(":"));
        if (line1_vector.size() == 1) {
            if (!target_feature_name_.empty())
                throw std::invalid_argument("Target feature name seems to exist 2 times!");
            target_feature_name_ = line1;
            continue;
        }
        if (line1_vector.size() > 2)
            throw std::invalid_argument("Colon two times in one line!");
        for(int i =0; i<2; ++i) {
            line1_vector[i] = std::regex_replace (line1_vector[i],std::regex(" *$"),"");
            line1_vector[i] = std::regex_replace (line1_vector[i],std::regex("^ *"),"");
        }
        if (line1_vector[0] == "case weight") {
            weight_index_ = (int)index++;
            continue;
        }
        if (line1_vector[1] == "continuous") {
            feature_names_.push_back(line1_vector[0]);
            feature_indices_.push_back(index++);
            continue;
        }
        if (line1_vector[1] == "ignore") {
            index++;
            continue;
        }
        if (line1_vector[0] == target_feature_name_) {
            target_feature_index_ = index++;
            std::vector<std::string> labels_vector;
            boost::split(labels_vector, line1_vector[1], boost::is_any_of(","));
            if (labels_vector.size() != 2)
                throw std::invalid_argument("Expecting exactly two class labels!");
            for (int i = 0; i < 2; ++i) {
                labels_vector[i] = std::regex_replace(labels_vector[i], std::regex(" *$"), "");
                labels_vector[i] = std::regex_replace(labels_vector[i], std::regex("^ *"), "");
            }
            negatives_label_ = labels_vector[0];
            positives_label_ = labels_vector[1];
            target_feature_found = true;
            continue;
        }
    }
    if (feature_indices_.empty())
        throw std::invalid_argument("Expecting positive number of features!");
    if (target_feature_name_.empty())
        throw std::invalid_argument("Cannot identify target feature!");
    if (!target_feature_found)
        throw std::invalid_argument("Cannot find target feature within the data!");
    if (negatives_label_.empty() || positives_label_.empty())
        throw std::invalid_argument("Cannot identify positives and negatives label!");
}

FeatureNames::FeatureNames(std::istream & is) {
    init_from_stream(is);
}

FeatureNames::FeatureNames(const std::string & file_name) {
    std::ifstream fs_data(file_name);
    if (!fs_data.is_open())
        throw std::runtime_error("Cannot open file: " + file_name);
    init_from_stream(fs_data);
    fs_data.close();
}

unsigned int FeatureNames::get_dim() const {
    return feature_indices_.size();
}

const std::vector<std::string> &FeatureNames::get_feature_names() const {
    return feature_names_;
}

const std::vector<unsigned int> &FeatureNames::get_feature_indices() const {
    return feature_indices_;
}

const std::string &FeatureNames::get_target_feature_name() const {
    return target_feature_name_;
}

unsigned int FeatureNames::get_target_feature_index() const {
    return target_feature_index_;
}

int FeatureNames::get_weight_index() const {
    return weight_index_;
}

const std::string &FeatureNames::get_negatives_label() const {
    return negatives_label_;
}

const std::string &FeatureNames::get_positives_label() const {
    return positives_label_;
}

FeatureNames::~FeatureNames() = default;


#endif
