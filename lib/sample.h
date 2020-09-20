#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include "feature_vector.h"

class Sample {
public:
    virtual unsigned int get_dim() const = 0;
    virtual unsigned int get_size() const = 0;
    virtual const std::pair<double, double> &get_neg_pos() = 0;
    virtual const std::shared_ptr<FeatureVector>& operator[](unsigned int) const = 0;
};

std::ostream &operator<<(std::ostream & stream, const Sample & sample) {
    if (sample.get_size()) {
        stream << sample[0];
    }
    for (unsigned int i = 1; i < sample.get_size(); ++i) {
        stream << ';' << *sample[i];
    }
    return stream;
}

#endif
