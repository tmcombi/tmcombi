#define BOOST_TEST_MODULE TestIO
#include <boost/test/included/unit_test.hpp>
#include "../lib/sample.h"

BOOST_AUTO_TEST_CASE( load_names_from_file )
{
    const std::string names_file("../data/tmc_paper_9/tmc_paper.names");
    const std::string data_file("../data/tmc_paper_9/tmc_paper.names");

    BOOST_TEST_MESSAGE("Creating names from file: " << names_file);
    std::ifstream fs_names(names_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(fs_names);
    fs_names.close();

    BOOST_CHECK_EQUAL( 4, 4 );
}
