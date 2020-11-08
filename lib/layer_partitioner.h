#ifndef LIB_LAYER_PARTITIONER_H_
#define LIB_LAYER_PARTITIONER_H_

#include <boost/graph/edmonds_karp_max_flow.hpp>
//#include <boost/graph/push_relabel_max_flow.hpp>
//#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include "layer.h"

#define DO_SLOW_CHECK

#ifdef DO_SLOW_CHECK
#include "../../glpk-4.65/src/glpk.h"
#endif



template <typename GraphType, typename IntType = double>
class LayerPartitioner {
public:
    LayerPartitioner();

    void set_layer(const std::shared_ptr<Layer> &);
    void set_graph(const std::shared_ptr<GraphType> &);

    /// return true in case layer is decomposable, dynamic_bitset has 0 for lower part and 1 for the upper part
    std::pair<boost::dynamic_bitset<>, bool> compute();

private:
    std::shared_ptr<Layer> pLayer_;
    std::shared_ptr<GraphType> pGraph_;
    unsigned int size_;
    std::vector<IntType> coefficients_;

    bool computed_fast_;
    boost::dynamic_bitset<> mask_fast_;
    bool decomposable_fast_;
    IntType optimal_obj_function_value_fast_;

#ifdef DO_SLOW_CHECK
    bool computed_slow_;
    boost::dynamic_bitset<> mask_slow_;
    bool decomposable_slow_;
    IntType optimal_obj_function_value_slow_;
#endif

    void compute_coefficients_from_int();
    void compute_coefficients_from_float();

    void compute_fast();

    typedef typename boost::graph_traits<GraphType>::vertex_descriptor vertex_descriptor;

    // todo: check whether std::vector or std::deque is faster
    typedef std::list<std::pair<vertex_descriptor,vertex_descriptor> > EdgesContainer;

    void mark_reachable(const vertex_descriptor &);



#ifdef DO_SLOW_CHECK
    IntType objective_function(const boost::dynamic_bitset<> &);
    void compute_slow();
#endif

};

template <typename GraphType, typename IntType>
LayerPartitioner<GraphType, IntType>::LayerPartitioner()
: pLayer_(nullptr), pGraph_(nullptr) , size_(0), coefficients_(0),
computed_fast_(false), decomposable_fast_(false),optimal_obj_function_value_fast_(0)
#ifdef DO_SLOW_CHECK
, computed_slow_(false), decomposable_slow_(false),optimal_obj_function_value_slow_(0)
#endif
{}

template <typename GraphType, typename IntType>
void LayerPartitioner<GraphType, IntType>::set_layer(const std::shared_ptr<Layer> & pLayer) {
    pLayer_ = pLayer;
    computed_fast_ = false;
#ifdef DO_SLOW_CHECK
    computed_slow_ = false;
#endif
    if (size_) {
        if (size_ != pLayer->size())
            throw std::runtime_error("Layer and Graph must correspond to each other");
    } else {
        size_ = pLayer->size();
        mask_fast_.resize(size_);
#ifdef DO_SLOW_CHECK
        mask_slow_.resize(size_);
#endif
    }

    if (pLayer_->weights_int()) {
        compute_coefficients_from_int();
    } else {
        compute_coefficients_from_float();
    }

}

template <typename GraphType, typename IntType>
void LayerPartitioner<GraphType, IntType>::compute_coefficients_from_int() {
    coefficients_.resize(size_);

    double dNegTotal=0, dPosTotal=0;
    std::tie(dNegTotal,dPosTotal) = pLayer_->get_neg_pos_counts();

    const auto iNegTotal = (IntType)dNegTotal;
    const auto iPosTotal = (IntType)dPosTotal;

    if ( (double)iNegTotal != dNegTotal || (double)iPosTotal != dPosTotal )
        throw std::runtime_error("within an integer case expecting a perfect rounding");

    for (unsigned int i = 0; i < size_; i++) {
        const auto & dNeg = (*pLayer_)[i]->get_weight_negatives();
        const auto & dPos = (*pLayer_)[i]->get_weight_positives();
        const auto iNeg = (IntType) dNeg;
        const auto iPos = (IntType) dPos;

        const IntType a = iNegTotal * iPos;
        const IntType b = iPosTotal * iNeg;

        if (a < 0 || b < 0)
            throw std::runtime_error("Overflow: try using another integer type");

        coefficients_[i] = a - b;
    }
}

template <typename GraphType, typename IntType>
void LayerPartitioner<GraphType, IntType>::compute_coefficients_from_float() {
    throw std::runtime_error("not yet implemented");
}


template <typename GraphType, typename IntType>
void LayerPartitioner<GraphType, IntType>::set_graph(const std::shared_ptr<GraphType> & pGraph) {
    pGraph_ = pGraph;
    computed_fast_ = false;
#ifdef DO_SLOW_CHECK
    computed_slow_ = false;
#endif
    if (size_) {
        if (size_ != boost::num_vertices(*pGraph))
            throw std::runtime_error("Layer and Graph must correspond to each other");
    } else {
        size_ = boost::num_vertices(*pGraph);
        mask_fast_.resize(size_);
#ifdef DO_SLOW_CHECK
        mask_slow_.resize(size_);
#endif
    }
}

template <typename GraphType, typename IntType>
std::pair<boost::dynamic_bitset<>, bool> LayerPartitioner<GraphType, IntType>::compute() {
#ifdef TIMERS
    {
        const std::clock_t time1 = std::clock();
#endif
        if (!computed_fast_) compute_fast();
#ifdef TIMERS
        const std::clock_t time2 = std::clock();
        std::cout << "Timers: " << (time2 - time1) / (CLOCKS_PER_SEC / 1000) << "ms - Compute splitting with a max flow"
                  << std::endl;
    }
#endif


#ifdef DO_SLOW_CHECK
#ifdef TIMERS
    {
        const std::clock_t time1 = std::clock();
#endif
        if (!computed_slow_) compute_slow();
#ifdef TIMERS
        const std::clock_t time2 = std::clock();
        std::cout << "Timers: " << (time2 - time1) / (CLOCKS_PER_SEC / 1000) << "ms - Compute splitting with a simplex"
                  << std::endl;
    }
#endif
    for (unsigned int i = 0; i< size_; i++) {
        if (mask_fast_[i] != mask_slow_[i]) {
            std::cout << "Warning: mask_fast_[" << i << "]=" << mask_fast_[i]
            << " and mask_slow_[" << i << "]=" << mask_slow_[i] << std::endl;
        }
    }
    if (computed_fast_ != computed_slow_)
        throw std::runtime_error("Slow and fast yield different results");
    if (mask_fast_ != mask_slow_)
        throw std::runtime_error("Slow and fast yield different results");
    if (decomposable_fast_ != decomposable_slow_)
        throw std::runtime_error("Slow and fast yield different results");
    if (optimal_obj_function_value_fast_ != optimal_obj_function_value_slow_)
        throw std::runtime_error("Slow and fast yield different results");
#endif

    return {mask_fast_, decomposable_fast_};
}


template <typename GraphType, typename IntType>
void LayerPartitioner<GraphType, IntType>::compute_fast() {

    IntType sum_positives = 0, sum_negatives = 0;

    auto s = boost::add_vertex(*pGraph_);
    auto t = boost::add_vertex(*pGraph_);

    auto capacity = get(boost::edge_capacity, *pGraph_);
    auto rev = get(boost::edge_reverse, *pGraph_);
    auto residual_capacity = get(boost::edge_residual_capacity, *pGraph_);

    EdgesContainer original_edges;
    typename boost::graph_traits<GraphType>::edge_iterator first, last;
    for ( boost::tie(first,last) = boost::edges(*pGraph_); first!=last; ++first ) {
        original_edges.push_back({boost::source(*first,*pGraph_), boost::target(*first,*pGraph_)});
    }

    typename boost::graph_traits<GraphType>::edge_descriptor e1, e2;
    bool ec1, ec2;
    for (unsigned int i = 0; i < size_; i++) {
        if (coefficients_[i] < 0) {
            std::tie(e1, ec1) = boost::add_edge(i,t,*pGraph_);
            std::tie(e2, ec2) = boost::add_edge(t,i,*pGraph_);
            if (!ec1 || !ec2)
                throw std::runtime_error("Cannot create graph edge");
            capacity[e1] = -coefficients_[i];
            capacity[e2] = 0;
            rev[e1]=e2;rev[e2]=e1;
            sum_negatives += coefficients_[i];
            if (sum_negatives > 0)
                throw std::runtime_error("Overflow: try using another integer type");
        } else if (coefficients_[i] > 0) {
            std::tie(e1, ec1) = boost::add_edge(s,i,*pGraph_);
            std::tie(e2, ec2) = boost::add_edge(i,s,*pGraph_);
            if (!ec1 || !ec2)
                throw std::runtime_error("Cannot create graph edge");
            capacity[e1] = coefficients_[i];
            capacity[e2] = 0;
            rev[e1]=e2;rev[e2]=e1;
            sum_positives += coefficients_[i];
            if (sum_positives < 0)
                throw std::runtime_error("Overflow: try using another integer type");
        }
    }
    if ( sum_negatives != -sum_positives )
        throw std::runtime_error("Unexpected error in objective function: sum of the coefficients must be zero");
    for(auto it = original_edges.begin(); it!= original_edges.end(); ++it) {
        std::tie(e1, ec1)  = boost::edge(it->first, it->second, *pGraph_);
        std::tie(e2, ec2)  = boost::add_edge(it->second, it->first, *pGraph_);
        if (!ec1 || !ec2)
            throw std::runtime_error("Cannot create graph edge");
        capacity[e1] = sum_positives;
        capacity[e2] = 0;
        rev[e1]=e2;rev[e2]=e1;
    }

    optimal_obj_function_value_fast_ = sum_positives - edmonds_karp_max_flow(*pGraph_,s,t);
    //optimal_obj_function_value_fast_ = sum_positives - push_relabel_max_flow(*pGraph_,s,t);
    //optimal_obj_function_value_fast_ = sum_positives - boykov_kolmogorov_max_flow(*pGraph_,s,t);
    decomposable_fast_ = optimal_obj_function_value_fast_ > 0;
    mask_fast_.reset();

    typename boost::graph_traits<GraphType>::out_edge_iterator ei, e_end;
    for( tie(ei,e_end) = boost::out_edges(s,*pGraph_); ei!=e_end; ++ei ) {
        const auto target = boost::target(*ei,*pGraph_);
        if ( residual_capacity[*ei]> 0 ) mask_fast_[target] = true;
    }

    boost::clear_vertex(s,*pGraph_);
    boost::clear_vertex(t,*pGraph_);
    boost::remove_vertex(s,*pGraph_);
    boost::remove_vertex(t,*pGraph_);

    for(auto it = original_edges.begin(); it!= original_edges.end(); ++it) {
        std::tie(e1, ec1)  = boost::edge(it->first, it->second, *pGraph_);
        std::tie(e2, ec2)  = boost::edge(it->second, it->first, *pGraph_);
        if (!ec1 || !ec2) throw std::runtime_error("Edges are expected to exist");
        if (residual_capacity[e1]==capacity[e1]) boost::remove_edge(e2,*pGraph_);
    }

    const auto marked = mask_fast_;
    for (unsigned int i = 0; i < size_; i++) {
        if (marked[i]) mark_reachable(i);
    }

    for(auto it = original_edges.begin(); it!= original_edges.end(); ++it) {
        std::tie(e2, ec2)  = boost::edge(it->second, it->first, *pGraph_);
        if (ec2) boost::remove_edge(e2,*pGraph_);
    }


#ifdef DO_SLOW_CHECK
    if ( optimal_obj_function_value_fast_ != objective_function(mask_fast_) ) {
        std::cout << "expected objective function due to max flow=" << optimal_obj_function_value_fast_ << std::endl;
        std::cout << "actual   objective function                =" << objective_function(mask_fast_) << std::endl;
        throw std::runtime_error("Unexpectedly changed objective function via transitive closure");
    }
#endif
    computed_fast_ = true;
}

template <typename GraphType, typename IntType>
void LayerPartitioner<GraphType, IntType>::mark_reachable(const vertex_descriptor & u) {
    typename boost::graph_traits<GraphType>::out_edge_iterator ei, e_end;
    for( tie(ei,e_end) = boost::out_edges(u,*pGraph_); ei!=e_end; ++ei ) {
        auto v = boost::target(*ei,*pGraph_);
        if (!mask_fast_[v]) {
            mask_fast_[v] = true;
            mark_reachable(v);
        }
    }
}

#ifdef DO_SLOW_CHECK
template <typename GraphType, typename IntType>
void LayerPartitioner<GraphType, IntType>::compute_slow() {
    glp_prob * lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MAX);
    glp_add_cols(lp, size_);
    for (int i = 0; (unsigned int)i < size_; i++) {
        glp_set_col_bnds(lp,i+1,GLP_DB,0,1);
        const auto coefficient = (double)coefficients_[i];
        glp_set_obj_coef(lp, i+1, coefficient);
    }
    const unsigned int n_rows = boost::num_edges(*pGraph_);
    if ( n_rows > 0 )
        glp_add_rows(lp, n_rows);
    std::vector<int> ia(2*n_rows +1);
    std::vector<int> ja(2*n_rows +1);
    std::vector<double> ar(2*n_rows +1);
    typename boost::graph_traits<GraphType>::edge_iterator first, last;
    int edge_index = 1;
    unsigned int coefficient_counter = 1;
    for ( boost::tie(first,last) = boost::edges(*pGraph_); first!=last; ++first ) {
        const vertex_descriptor u = boost::source(*first, *pGraph_);
        const vertex_descriptor v = boost::target(*first, *pGraph_);
        ia[coefficient_counter] = edge_index; ja[coefficient_counter] = u+1; ar[coefficient_counter] = 1;
        coefficient_counter++;
        ia[coefficient_counter] = edge_index; ja[coefficient_counter] = v+1; ar[coefficient_counter] = -1;
        coefficient_counter++;
        glp_set_row_bnds(lp,edge_index,GLP_UP,0,0);
        edge_index++;
    }
    glp_load_matrix(lp,2*n_rows,&ia[0],&ja[0],&ar[0]);
    glp_smcp smcp;
    glp_init_smcp (&smcp);
    smcp.msg_lev = GLP_MSG_OFF;
    glp_simplex(lp,&smcp);
    optimal_obj_function_value_slow_ = (IntType)glp_get_obj_val(lp);
    decomposable_slow_ = optimal_obj_function_value_slow_ > 0;
    if (decomposable_slow_) {
        for (int i = 0; (unsigned int)i < size_; i++) {
            const double b = glp_get_col_prim(lp,i+1);
            mask_slow_[i] = b == 1;
        }
    }
    glp_delete_prob(lp);
    computed_slow_ = true;
}

template <typename GraphType, typename IntType>
IntType LayerPartitioner<GraphType, IntType>::objective_function(const boost::dynamic_bitset<> & bs) {
    IntType result = 0;
    for (unsigned int i = 0; i < size_; i++) {
        if (bs[i]) result += coefficients_[i];
    }
    return result;
}

#endif

#endif
