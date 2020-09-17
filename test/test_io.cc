#define BOOST_TEST_MODULE TestIO
#include <boost/test/included/unit_test.hpp>
#include "../lib/sample.h"

BOOST_AUTO_TEST_CASE( load_names_from_file )
{
    const std::string names_file("data/tmc_paper_9/tmc_paper.names");
    const std::string data_file("data/tmc_paper_9/tmc_paper.data");

    std::shared_ptr<FeatureNames> pFN;
    BOOST_TEST_MESSAGE("Creating names from file: " << names_file);
    std::ifstream fs_names(names_file);
    if (fs_names.is_open())
    {
        pFN = std::make_shared<FeatureNames>(fs_names);
        fs_names.close();
    } else {
        BOOST_FAIL("Cannot open names file!");
    }

    ContainerSample sample(pFN);
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::ifstream fs_data(data_file);
    if (fs_data.is_open())
    {
        sample.push_from_stream(fs_data);
        fs_names.close();
        BOOST_TEST_MESSAGE("Resulting sample: " << sample);
        BOOST_CHECK_EQUAL(sample.get_dim(), 2);
        BOOST_CHECK_EQUAL(sample.get_size(), 4);
    } else {
        BOOST_FAIL("Cannot open data file!");
    }

}
