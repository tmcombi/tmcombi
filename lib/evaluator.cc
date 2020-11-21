#define BOOST_TEST_MODULE lib_test_evaluator
#include <boost/test/included/unit_test.hpp>

#include "border_system_creator.h"
#include "evaluator.h"
#include "layer_partitioning.h"
#include "sample_creator.h"

#define PUSH(x,y,neg,pos,pFV,pS2E) pFV = std::make_shared<FeatureVector>(std::vector<double>({x,y})); \
(*pFV).inc_weight_negatives(neg).inc_weight_positives(pos);                                  \
pS2E->push(pFV);


BOOST_AUTO_TEST_CASE( evaluator_basics ) {
    const std::string names_file("data/4layers_36points/4layers_36points.names");
    const std::string data_file("data/4layers_36points/4layers_36points.data");
    BOOST_TEST_MESSAGE("Creating sample from file: " << data_file);
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(names_file);
    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);
    std::shared_ptr<Sample> pSample = sample_creator.from_file(data_file);
    BOOST_TEST_MESSAGE("Resulting sample: " << *pSample);

    boost::dynamic_bitset<> db4(36);
    boost::dynamic_bitset<> db3(27);
    boost::dynamic_bitset<> db2(19);
    db4[27] = db4[28] = db4[29] = db4[30] = db4[31] = db4[32] = db4[33] = db4[34] = db4[35] = true;
    db3[19] = db3[20] = db3[21] = db3[22] = db3[23] = db3[24] = db3[25] = db3[26] = true;
    db2[12] = db2[13] = db2[14] = db2[15] = db2[16] = db2[17] = db2[18] = true;

    std::shared_ptr<LayerPartitioning> pLD = std::make_shared<LayerPartitioning>();
    pLD->push_back(pSample);

    BOOST_TEST_MESSAGE("Create layer partitioning via splitting the lowest layer three times into 4 layers");
    { auto it = pLD->begin(); pLD->split_layer(it, db4); }
    { auto it = pLD->begin(); pLD->split_layer(it, db3); }
    { auto it = pLD->begin(); pLD->split_layer(it, db2); }

    BOOST_CHECK_EQUAL(pLD->consistent(),true);
    auto pLayer1 = *pLD->begin();
    auto pLayer2 = *(pLD->begin() + 1);
    auto pLayer3 = *(pLD->begin() + 2);
    auto pLayer4 = *(pLD->begin() + 3);

    BOOST_TEST_MESSAGE("Creating border system");
    std::shared_ptr<BorderSystemCreator> pBSC = std::make_shared<BorderSystemCreator>();
    std::shared_ptr<BorderSystem> pBS = pBSC->from_layer_partitioning(pLD);
    BOOST_CHECK_EQUAL(pBS.use_count(), 1);
    std::vector<double> p;  std::pair<double, double> conf;

    p = {10,2}; conf = {1.0/6.0,1.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {16,1}; conf = {1.0/3.0,1.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {7,10}; conf = {2.0/3.0,2.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {11,10}; conf = {5.0/6.0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {4,6}; conf = {0,1.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {7,13}; conf = {5.0/6.0,1}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {14.5,0.5}; conf = {0,1.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {-1,16}; conf = {0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {-1,18}; conf = {0,1}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {12.5,1.5}; conf = {1.0/6.0,1.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {4,8}; conf = {1.0/6.0,2.0/3.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {3,13}; conf = {1.0/6.0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {1,18};  conf = {1.0/6.0,1}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {15,5.5}; conf = {1.0/3.0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {18,1}; conf = {1.0/3.0,1}; BOOST_CHECK(pBS->confidence_interval(p) == conf);
    p = {8,12}; conf = {2.0/3.0,5.0/6.0}; BOOST_CHECK(pBS->confidence_interval(p) == conf);

    std::shared_ptr<FeatureVector> pFV(nullptr);
    auto pS2E = std::make_shared<Sample>(2);
    PUSH(10,2,1,4,pFV,pS2E); /// {1/6,1/6} TP=0 FP=0 FN=4 TN=1 Confl=1*4/2=2
    PUSH(16,1,2,3,pFV,pS2E); /// {1/3,1/3} TP=0 FP=0 FN=3 TN=2 Confl=2*3/2+2*(4)+3*(0)=11
    PUSH(7,10,3,2,pFV,pS2E); /// {2/3,2/3} TP=2 FP=3 FN=0 TN=0 Confl=3*2/2+3*(4+3)+2*(0)=24
    PUSH(11,10,4,1,pFV,pS2E); /// {5/6,5/6} TP=1 FP=4 FN=0 TN=0 Confl=4*1/2+4*(4+3+2)+1*(0)=38
    PUSH(4,6,3,2,pFV,pS2E); /// {0,1/6} TP=0 FP=0 FN=2 TN=3 Confl=3*2/2+3*(4/2)+2*(1/2+2+3+4)=28
    PUSH(7,13,2,3,pFV,pS2E); /// {5/6,1} TP=3 FP=2 FN=0 TN=0 Confl=2*3/2+2*(4+3+2+1/2+2)+3*(4/2)=32
    PUSH(14.5,0.5,1,4,pFV,pS2E); /// {0,1/3} TP=0 FP=0 FN=4 TN=1 Confl=1*4/2+1*(4/2+3/2+2/2)+4*(1/2+2/2+3+4+3/2+2)=54.5
    PUSH(-1,16,2,3,pFV,pS2E); /// {0,5/6} TP=1.5 FP=1 FN=1.5 TN=1 Confl=2*3/2+2*(4/2+3/2+2/2+1/2+2/2+3/2+4/2)+3*(1/2+2/2+3/2+4/2+3/2+2/2+1/2)=46
    PUSH(-1,18,3,2,pFV,pS2E); /// {0,1} TP=1 FP=1.5 FN=1 TN=1.5 Confl=3*2/2+3*(4/2+3/2+2/2+1/2+2/2+3/2+4/2+3/2)+2*(1/2+2/2+3/2+4/2+3/2+2/2+1/2+2/2)=54
    PUSH(12.5,1.5,4,1,pFV,pS2E); /// {1/6,1/3} TP=0 FP=0 FN=1 TN=4 Confl=4*1/2+4*(4/2+3/2+2/2+4/2+3/2+2/2)+1*(1/2+2/2+3+4+3/2+2+1/2+2/2+3/2)=53
    PUSH(4,8,3,2,pFV,pS2E); /// {1/6,2/3} TP=1 FP=1.5 FN=1 TN=1.5 Confl=3*2/2+3*(4/2+3/2+2/2+2/2+4/2+3/2+2/2+1/2)+2*(1/2+2/2+3/2+4+3/2+2+1/2+2/2+3/2+4/2)=65.5
    PUSH(3,13,2,3,pFV,pS2E); /// {1/6,5/6} TP=1.5 FP=1 FN=1.5 TN=1 Confl=2*3/2+2*(4+3+2+1+2+3+4+3+2+1+2)/2+3*(1+2+3+4+3+2+1+2+3+4+3)/2=72
    PUSH(1,18,1,4,pFV,pS2E); /// {1/6,1} TP=2 FP=0.5 FN=2 TN=0.5 Confl=1*4/2+1*(4+3+2+1+2+3+4+3+2+1+2+3)/2+4*(1+2+3+4+3+2+1+2+3+4+3+2)/2=77
    PUSH(15,5.5,2,3,pFV,pS2E); /// {1/3,5/6} TP=1.5 FP=1 FN=1.5 TN=1 Confl=2*3/2+2*(2*4+3+2+1+2*2+3+4+3+2+1+2+3+4)/2+3*(2+3+4+2+1+2+3+4+3+2+1)/2=83.5
    PUSH(18,1,3,2,pFV,pS2E); /// {1/3,1} TP=1 FP=1.5 FN=1 TN=1.5 Confl=3*2/2+3*(2*4+3+2+1+2*2+3+4+3+2+1+2+3+4+3)/2+2*(2+3+4+2+1+2+3+4+3+2+1+2)/2=96.5
    PUSH(8,12,4,1,pFV,pS2E); /// {2/3,5/6} TP=1 FP=4 FN=0 TN=0 Confl=4*1/2+4*(4+3+2/2+1/2+2+3/2+4+3/2+2/2+1+2/2+3/2+4/2+3/2+2/2)+1*(3/2+4/2+2/2+2/2+3/2+3/2+2/2+1/2+2/2+3/2)=120.5

    auto pEvaluator = std::make_shared<Evaluator>();
    (*pEvaluator).set_sample(pS2E).set_border_system(pBS);

    std::pair<std::pair<double, double>, std::pair<double, double>> confusion_matrix({{16.5,21}, {23.5,19}});
    BOOST_CHECK(pEvaluator->get_confusion_matrix() == confusion_matrix);
    BOOST_CHECK_EQUAL(pEvaluator->get_error_rate(), (21.0+23.5)/80.0);
    BOOST_CHECK_EQUAL(pEvaluator->get_accuracy(), (16.5+19.0)/80.0);

    const double num_conflicts = 2+11+24+38+28+32+54.5+46+54+53+65.5+72+77+83.5+96.5+120.5;
    double neg, pos;
    std::tie(neg,pos) = pS2E->get_neg_pos_counts();

    BOOST_CHECK_EQUAL(pEvaluator->get_ranking_conflicts(), num_conflicts);
    BOOST_CHECK_EQUAL(pEvaluator->get_roc_error(), num_conflicts/(neg*pos));
    BOOST_CHECK_EQUAL(pEvaluator->get_roc(), 1-num_conflicts/(neg*pos));

    pEvaluator->evaluate_data_file(std::cout,data_file, pFN);
}
