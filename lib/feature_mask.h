#ifndef LIB_FEATURE_MASK_H_
#define LIB_FEATURE_MASK_H_

#include <boost/exception/to_string.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/property_tree/ptree.hpp>
#include <type_traits>
#include <utility>

class FeatureMask {
public:
    explicit FeatureMask(size_t dim = 0);
    explicit FeatureMask(const boost::dynamic_bitset<> &);
    explicit FeatureMask(boost::dynamic_bitset<>, boost::dynamic_bitset<>);
    explicit FeatureMask(const std::string &);
    explicit FeatureMask(const std::string &, const std::string &);

    size_t dim() const;

    size_t find_first() const;
    size_t find_next(size_t) const;
    size_t count() const;

    void resize(size_t);
    boost::dynamic_bitset<>::reference operator[](size_t);
    bool operator[](size_t) const;
    boost::dynamic_bitset<>::reference sign(size_t);
    bool sign(size_t) const;

    std::pair<std::string,std::string> to_strings() const;

    explicit FeatureMask(const boost::property_tree::ptree &);
    const FeatureMask & dump_to_ptree(boost::property_tree::ptree &) const;

    //todo: remove deprecated
    boost::dynamic_bitset<> get_index_mask();
    boost::dynamic_bitset<> get_sign_mask();

private:
    boost::dynamic_bitset<> mask_;
    boost::dynamic_bitset<> sign_;
};

FeatureMask::FeatureMask(size_t dim) : mask_(dim), sign_(dim) {
}

FeatureMask::FeatureMask(const boost::dynamic_bitset<> & bs) : mask_(bs), sign_(bs.size()){
}

FeatureMask::FeatureMask(boost::dynamic_bitset<>  bs1, boost::dynamic_bitset<>  bs2) :
mask_(std::move(bs1)), sign_(std::move(bs2)) {
    if (mask_.size() != sign_.size())
        throw std::runtime_error("Bitsets should be of the same size");
}

FeatureMask::FeatureMask(const std::string & s) : mask_(s), sign_(s.size()) {
}

FeatureMask::FeatureMask(const std::string & s1, const std::string & s2) : mask_(s1), sign_(s2) {
    if (mask_.size() != sign_.size())
        throw std::runtime_error("Bitsets should be of the same size");
}

size_t FeatureMask::dim() const {
    return mask_.size();
}

size_t FeatureMask::find_first() const {
    return mask_.find_first();
}

size_t FeatureMask::find_next(const size_t i) const {
    return mask_.find_next(i);
}

size_t FeatureMask::count() const {
    return mask_.count();
}

void FeatureMask::resize(const size_t i) {
    mask_.resize(i);
    sign_.resize(i);
}

boost::dynamic_bitset<>::reference FeatureMask::operator[](size_t i) {
    return mask_[i];
}

bool FeatureMask::operator[](size_t i) const {
    return mask_[i];
}

boost::dynamic_bitset<>::reference FeatureMask::sign(size_t i) {
    return sign_[i];
}

bool FeatureMask::sign(size_t i) const {
    return sign_[i];
}

std::pair<std::string, std::string> FeatureMask::to_strings() const {
    return std::make_pair(boost::to_string(mask_),boost::to_string(sign_));
}

FeatureMask::FeatureMask(const boost::property_tree::ptree & pt) :
mask_(pt.get<std::string>("mask")),
sign_(pt.get<std::string>("sign")) {
    if ( pt.get<std::string>("type") != "FeatureMask" )
        throw std::runtime_error("Expecting configuration of type FeatureMask");
    if (mask_.size() != sign_.size())
        throw std::runtime_error("Bitsets should be of the same size");
    if(dim() != pt.get<double>("dim"))
        throw std::runtime_error("Ptree is not consistent");
}

const FeatureMask & FeatureMask::dump_to_ptree(boost::property_tree::ptree & pt) const {
    using boost::property_tree::ptree;
    pt.put("type", "FeatureMask");
    const size_t dim = this->dim();
    pt.put("dim", dim);
    const auto s = to_strings();
    pt.put("mask", s.first);
    pt.put("sign", s.second);
    return *this;
}

boost::dynamic_bitset<> FeatureMask::get_index_mask() {
    return mask_;
}

boost::dynamic_bitset<> FeatureMask::get_sign_mask() {
    return sign_;
}

#endif
