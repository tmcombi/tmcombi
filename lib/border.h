#ifndef LIB_BORDER_H_
#define LIB_BORDER_H_

#include "sample.h"

// redefine eg_pos_counts vs. class Sample

class Border : virtual public Sample {
public:
    explicit Border(unsigned int); // unsigned int = dimension

    void set_neg_pos_counts(const std::pair<double, double> &);
    const std::pair<double, double> & get_neg_pos_counts() override;

private:
    bool neg_pos_counts_set_;
};

Border::Border(unsigned int dim) : Sample(dim), neg_pos_counts_set_(false) {
}

void Border::set_neg_pos_counts(const std::pair<double, double> & np) {
    total_neg_pos_counts_ = np;
    neg_pos_counts_set_ = true;
}

const std::pair<double, double> &Border::get_neg_pos_counts() {
    if (!neg_pos_counts_set_)
        throw std::domain_error("In case of Border neg_pos_counts should be set before use!");
    return Sample::get_neg_pos_counts();
}

#endif
