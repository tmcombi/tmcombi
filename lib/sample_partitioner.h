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
    bool feasible(const boost::dynamic_bitset<> &);
    double objective_function(const boost::dynamic_bitset<> &);
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
    if (!computed_slow_) compute_slow();
    marks_fast_ = marks_slow_;
    decomposable_fast_ = decomposable_slow_;
    optimal_obj_function_value_fast_ = optimal_obj_function_value_slow_;
    computed_fast_ = true;
}

#ifdef DO_SLOW_CHECK
template<typename GraphType>
void SamplePartitioner<GraphType>::compute_slow() {
    glp_prob * lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MAX);
    glp_add_cols(lp, size_);
    for (int i = 0; i < size_; i++) {
        glp_set_col_bnds(lp,i+1,GLP_DB,0,1);
        const double c = pSample_->get_neg_pos_counts().first * (*pSample_)[i]->get_weight_positives()
                -pSample_->get_neg_pos_counts().second * (*pSample_)[i]->get_weight_negatives();
        glp_set_obj_coef(lp, i+1, c);
    }
    const unsigned int n_rows = boost::num_edges(*pGraph_);
    glp_add_rows(lp, n_rows);
    std::vector<int> ia(2*n_rows +1);
    std::vector<int> ja(2*n_rows +1);
    std::vector<double> ar(2*n_rows +1);
    typename boost::graph_traits<GraphType>::edge_iterator first, last;
    int edge_index = 1;
    unsigned int coefficient_counter = 1;
    for ( boost::tie(first,last) = boost::edges(*pGraph_); first!=last; ++first ) {
        const typename boost::graph_traits<GraphType>::vertex_descriptor u = boost::source(*first, *pGraph_);
        const typename boost::graph_traits<GraphType>::vertex_descriptor v = boost::target(*first, *pGraph_);
        ia[coefficient_counter] = edge_index; ja[coefficient_counter] = u+1; ar[coefficient_counter] = 1;
        coefficient_counter++;
        ia[coefficient_counter] = edge_index; ja[coefficient_counter] = v+1; ar[coefficient_counter] = -1;
        coefficient_counter++;
        glp_set_row_bnds(lp,edge_index,GLP_UP,0,0);
        edge_index++;
    }
    glp_load_matrix(lp,2*n_rows,&ia[0],&ja[0],&ar[0]);
    glp_simplex(lp,nullptr);
    optimal_obj_function_value_slow_ = glp_get_obj_val(lp);
    decomposable_slow_ = optimal_obj_function_value_slow_ > 0;
    if (decomposable_slow_) {
        for (int i = 0; i < size_; i++) {
            const double b = glp_get_col_prim(lp,i+1);
            marks_slow_[i] = b == 1;
        }
    }
    glp_delete_prob(lp);
    computed_slow_ = true;
}
#endif

#ifdef DO_VERY_SLOW_CHECK
template<typename GraphType>
void SamplePartitioner<GraphType>::compute_very_slow() {
    std::vector<boost::dynamic_bitset<> > tempSubsets;
    std::vector<boost::dynamic_bitset<> > allSubsets;
    tempSubsets.push_back(boost::dynamic_bitset<>(0));
    for (unsigned int i = 0; i < size_; i++) {
        allSubsets.clear();
        for (unsigned int j = 0; j < tempSubsets.size(); j++) {
            { boost::dynamic_bitset<> bs = tempSubsets[j]; bs.push_back(false); allSubsets.push_back(bs); }
            { boost::dynamic_bitset<> bs = tempSubsets[j]; bs.push_back(true); allSubsets.push_back(bs); }
        }
        tempSubsets = allSubsets;
    }
    optimal_obj_function_value_very_slow_ = -std::numeric_limits<double>::max();

    for (unsigned int j = 0; j < allSubsets.size(); j++) {
        const bool feasible = this->feasible(allSubsets[j]);
        if (feasible) {
            const double obj_function = objective_function(allSubsets[j]);
            if (obj_function > optimal_obj_function_value_very_slow_) {
                optimal_obj_function_value_very_slow_ = obj_function;
                marks_very_slow_ = allSubsets[j];
            }
        }
    }
    decomposable_very_slow_ = optimal_obj_function_value_very_slow_ > 0;
    computed_very_slow_ = true;
}

template<typename GraphType>
bool SamplePartitioner<GraphType>::feasible(const boost::dynamic_bitset<> &db) {
    typename boost::graph_traits<GraphType>::edge_iterator first, last;
    for ( boost::tie(first,last) = boost::edges(*pGraph_); first!=last; ++first ) {
        const typename boost::graph_traits<GraphType>::vertex_descriptor u = boost::source(*first,*pGraph_);
        const typename boost::graph_traits<GraphType>::vertex_descriptor v = boost::target(*first,*pGraph_);
        if (db[u] && !db[v]) return false;
    }
    return true;
}

template<typename GraphType>
double SamplePartitioner<GraphType>::objective_function(const boost::dynamic_bitset<> & bs) {
    double result = 0;
    for (unsigned int i = 0; i < size_; i++) {
        if (bs[i]) result +=
                pSample_->get_neg_pos_counts().first * (*pSample_)[i]->get_weight_positives()
                -pSample_->get_neg_pos_counts().second * (*pSample_)[i]->get_weight_negatives();
    }
    return result;
}

#endif



#endif
