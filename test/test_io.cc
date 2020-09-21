#define BOOST_TEST_MODULE TestIO
#include <boost/test/included/unit_test.hpp>
#include "../lib/sample_creator.h"

BOOST_AUTO_TEST_CASE( load_from_files )
{
    const std::string names_file("data/tmc_paper_9/tmc_paper.names");
    const std::string data_file("data/tmc_paper_9/tmc_paper.data");

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
}
