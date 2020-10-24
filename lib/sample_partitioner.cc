#define BOOST_TEST_MODULE lib_test_sample_partitioner
#include <boost/test/included/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "sample_creator.h"
#include "induced_graph.h"

#include "../../glpk-4.65/src/glpk.h"

BOOST_AUTO_TEST_CASE( test_glpk ) {
    glp_prob *lp;
    lp = glp_create_prob();
    glp_delete_prob(lp);
}
