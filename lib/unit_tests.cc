#define BOOST_TEST_MODULE lib_unit_tests

#include <exception>
bool is_critical(const std::exception& ) { return true; }

#define TMC_UNIT_TESTS

#include "border.cc"
#include "border_system.cc"
#include "classifier_creator_dispatch_ptree.cc"
#include "classifier_creator_fs_graph.cc"
#include "classifier_creator_fs_n_fold.cc"
#include "classifier_creator_train_tmc.cc"
#include "classifier_tmc.cc"
#include "classifier_transformed_features.cc"
#include "evaluator.cc"
#include "feature_names.cc"
#include "feature_transform_subset.cc"
#include "feature_vector.cc"
#include "graph_creator.cc"
#include "layer_partitioner.cc"
#include "layer_partitioning.cc"
#include "layer_partitioning_creator.cc"
#include "sample.cc"
#include "sample_creator.cc"
#include "statistics.cc"
#include "timers.cc"
