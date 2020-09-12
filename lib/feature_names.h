#ifndef LIB_FEATURE_NAMES_H_
#define LIB_FEATURE_NAMES_H_

class FeatureNames {
public:
    explicit FeatureNames(const std::stringstream &);
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
    unsigned int dim_;
    std::vector<std::string> feature_names_;
    std::vector<unsigned int> feature_indices_;
    std::string target_feature_name_;
    unsigned int target_feature_index_;
    std::string negatives_label_;
    std::string positives_label_;
    int weight_index_;
};

FeatureNames::FeatureNames(const std::stringstream & ss):
dim_(0), weight_index_(-1)
{
}

unsigned int FeatureNames::get_dim() const {
    return dim_;
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
