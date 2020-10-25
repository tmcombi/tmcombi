#ifndef LIB_SAMPLE_PARTITIONER_H_
#define LIB_SAMPLE_PARTITIONER_H_

#define DO_SLOW_CHECK
#define DO_VERY_SLOW_CHECK

#ifdef DO_SLOW_CHECK
#include "../../glpk-4.65/src/glpk.h"
#endif

#include "sample.h"
#include "induced_graph.h"

template <typename GraphType>
class SamplePartitioner {
public:
    SamplePartitioner();
    void set_sample(const std::shared_ptr<Sample> &);
    void set_graph(const std::shared_ptr<GraphType> &);

    /// return true in case sample is decomposable, dynamic_bitset has 0 for lower part and 1 for upper part
    std::pair<boost::dynamic_bitset<>, bool> compute();

private:
    std::shared_ptr<Sample> pSample_;
    std::shared_ptr<GraphType> pGraph_;
    unsigned int size_;

    bool computed_fast_;
    boost::dynamic_bitset<> marks_fast_;
    bool decomposable_fast_;
    double optimal_obj_function_value_fast_;

#ifdef DO_SLOW_CHECK
    bool computed_slow_;
    boost::dynamic_bitset<> marks_slow_;
    bool decomposable_slow_;
    double optimal_obj_function_value_slow_;
#endif

#ifdef DO_VERY_SLOW_CHECK
    bool computed_very_slow_;
    boost::dynamic_bitset<> marks_very_slow_;
    bool decomposable_very_slow_;
    double optimal_obj_function_value_very_slow_;
#endif

    void compute_fast();

#ifdef DO_SLOW_CHECK
    void compute_slow();
#endif

#ifdef DO_VERY_SLOW_CHECK
    void compute_very_slow();
#endif

};

template<typename GraphType>
SamplePartitioner<GraphType>::SamplePartitioner()
: pSample_(nullptr), pGraph_(nullptr) , size_(0),
computed_fast_(false), decomposable_fast_(false),optimal_obj_function_value_fast_(0)
#ifdef DO_SLOW_CHECK
, computed_slow_(false), decomposable_slow_(false),optimal_obj_function_value_slow_(0)
#endif
#ifdef DO_VERY_SLOW_CHECK
, computed_very_slow_(false), decomposable_very_slow_(false),optimal_obj_function_value_very_slow_(0)
#endif
{}

template<typename GraphType>
void SamplePartitioner<GraphType>::set_sample(const std::shared_ptr<Sample> &pSample) {
    pSample_ = pSample;
    computed_fast_ = false;
#ifdef DO_SLOW_CHECK
    computed_slow_ = false;
#endif
#ifdef DO_VERY_SLOW_CHECK
    computed_very_slow_ = false;
#endif
    if (size_) {
        if (size_ != pSample->size())
            throw std::runtime_error("Sample and Graph must correspond to each other");
    } else {
        size_ = pSample->size();
        marks_fast_.resize(size_);
#ifdef DO_SLOW_CHECK
        marks_slow_.resize(size_);
#endif
#ifdef DO_VERY_SLOW_CHECK
        marks_very_slow_.resize(size_);
#endif
    }
}

template<typename GraphType>
void SamplePartitioner<GraphType>::set_graph(const std::shared_ptr<GraphType> &pGraph) {
    pGraph_ = pGraph;
    computed_fast_ = false;
#ifdef DO_SLOW_CHECK
    computed_slow_ = false;
#endif
#ifdef DO_VERY_SLOW_CHECK
    computed_very_slow_ = false;
#endif
    if (size_) {
        if (size_ != boost::num_vertices(*pGraph))
            throw std::runtime_error("Sample and Graph must correspond to each other");
    } else {
        size_ = boost::num_vertices(*pGraph);
        marks_fast_.resize(size_);
#ifdef DO_SLOW_CHECK
        marks_slow_.resize(size_);
#endif
#ifdef DO_VERY_SLOW_CHECK
        marks_very_slow_.resize(size_);
#endif
    }
}

template<typename GraphType>
std::pair<boost::dynamic_bitset<>, bool> SamplePartitioner<GraphType>::compute() {
    if (!computed_fast_) compute_fast();

#ifdef DO_SLOW_CHECK
    if (!computed_slow_) compute_slow();
    if (computed_fast_ != computed_slow_)
        throw std::runtime_error("Slow and fast yield different results");
    if (marks_fast_ != marks_slow_)
        throw std::runtime_error("Slow and fast yield different results");
    if (decomposable_fast_ != decomposable_slow_)
        throw std::runtime_error("Slow and fast yield different results");
    if (optimal_obj_function_value_fast_ != optimal_obj_function_value_slow_)
        throw std::runtime_error("Slow and fast yield different results");
#endif

#ifdef DO_VERY_SLOW_CHECK
    if (!computed_very_slow_) compute_very_slow();
    if (computed_fast_ != computed_very_slow_)
        throw std::runtime_error("Very slow and fast yield different results");
    if (marks_fast_ != marks_very_slow_)
        throw std::runtime_error("Very slow and fast yield different results");
    if (decomposable_fast_ != decomposable_very_slow_)
        throw std::runtime_error("Very slow and fast yield different results");
    if (optimal_obj_function_value_fast_ != optimal_obj_function_value_very_slow_)
        throw std::runtime_error("Very slow and fast yield different results");
#endif

    return {marks_fast_, decomposable_fast_};
}


template<typename GraphType>
void SamplePartitioner<GraphType>::compute_fast() {
    computed_fast_ = true;
}

#ifdef DO_SLOW_CHECK
template<typename GraphType>
void SamplePartitioner<GraphType>::compute_slow() {
    computed_slow_ = true;
}
#endif

#ifdef DO_VERY_SLOW_CHECK
template<typename GraphType>
void SamplePartitioner<GraphType>::compute_very_slow() {
    computed_very_slow_ = true;
}
#endif



#endif
