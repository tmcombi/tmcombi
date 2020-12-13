#define BOOST_TEST_MODULE lib_train_classifier_tmc
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "feature_names.h"
#include "train_classifier_tmc.h"

BOOST_AUTO_TEST_CASE( train_classifier_tmc_basics )
{
    const std::string names_file("data/4layers_36points/4layers_36points.names");
    const std::string data_file("data/4layers_36points/4layers_36points.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 36);

    std::shared_ptr<TrainClassifier> pTC = std::make_shared<TrainClassifierTmc>(pSample);
    pTC->train();

    std::shared_ptr<Classifier> pClTmc = pTC->get_classifier();

    std::vector<double> p;  std::pair<double, double> conf;
    p = {10,2}; conf = {1.0/6.0,1.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {16,1}; conf = {1.0/3.0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {7,10}; conf = {2.0/3.0,2.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {11,10}; conf = {5.0/6.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {4,6}; conf = {0,1.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.second);
    p = {7,13}; conf = {5.0/6.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {14.5,0.5}; conf = {0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8)/(double)(50+10+16+8));
    p = {-1,16}; conf = {0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {-1,18}; conf = {0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {12.5,1.5}; conf = {1.0/6.0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8)/(double)(50+10+16+8));
    p = {4,8}; conf = {1.0/6.0,2.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18)/(double)(50+10+16+8+9+18));
    p = {3,13}; conf = {1.0/6.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {1,18};  conf = {1.0/6.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {15,5.5}; conf = {1.0/3.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (8+18+45)/(double)(16+8+9+18+9+45));
    p = {18,1}; conf = {1.0/3.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (8+18+45)/(double)(16+8+9+18+9+45));
    p = {8,12}; conf = {2.0/3.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (18+45)/(double)(9+18+9+45));
}

BOOST_AUTO_TEST_CASE( train_classifier_tmc_ptree )
{
    const std::string names_file("data/4layers_36points/4layers_36points.names");
    const std::string data_file("data/4layers_36points/4layers_36points.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);

    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);

    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);
    BOOST_CHECK_EQUAL(pSample->dim(), 2);
    BOOST_CHECK_EQUAL(pSample->size(), 36);

    std::shared_ptr<TrainClassifier> pTC = std::make_shared<TrainClassifierTmc>(pSample);
    pTC->train();
    boost::property_tree::ptree pt;
    pTC->dump_configuration_to_ptree(pt);
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());


    std::shared_ptr<Classifier> pClTmc = std::make_shared<ClassifierTmc>(pt);

    std::vector<double> p;  std::pair<double, double> conf;
    p = {10,2}; conf = {1.0/6.0,1.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {16,1}; conf = {1.0/3.0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {7,10}; conf = {2.0/3.0,2.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {11,10}; conf = {5.0/6.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {4,6}; conf = {0,1.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.second);
    p = {7,13}; conf = {5.0/6.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), conf.first);
    p = {14.5,0.5}; conf = {0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8)/(double)(50+10+16+8));
    p = {-1,16}; conf = {0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {-1,18}; conf = {0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {12.5,1.5}; conf = {1.0/6.0,1.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8)/(double)(50+10+16+8));
    p = {4,8}; conf = {1.0/6.0,2.0/3.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18)/(double)(50+10+16+8+9+18));
    p = {3,13}; conf = {1.0/6.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {1,18};  conf = {1.0/6.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (10+8+18+45)/(double)(50+10+16+8+9+18+9+45));
    p = {15,5.5}; conf = {1.0/3.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (8+18+45)/(double)(16+8+9+18+9+45));
    p = {18,1}; conf = {1.0/3.0,1}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (8+18+45)/(double)(16+8+9+18+9+45));
    p = {8,12}; conf = {2.0/3.0,5.0/6.0}; BOOST_CHECK(pClTmc->confidence_interval(p) == conf);
    BOOST_CHECK_EQUAL(pClTmc->confidence(p), (18+45)/(double)(9+18+9+45));
}
