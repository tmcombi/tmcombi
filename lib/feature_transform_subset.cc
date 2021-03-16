#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_feature_transform_subset
#endif
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "feature_transform_subset.h"

BOOST_AUTO_TEST_SUITE( feature_transform_subset )

BOOST_AUTO_TEST_CASE( basics )
{
    std::vector<double> v_in = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<double> v_out = {1, -4, -7, 9}, v_out1, v_out2, v_out3;
    std::string index_mask_str = "01010010010";
    std::string sign_mask_str = "00010010000";
    boost::dynamic_bitset<> index_mask(index_mask_str);
    boost::dynamic_bitset<> sign_mask(sign_mask_str);
    size_t dim_in=index_mask.size();
    size_t dim_out=index_mask.count();
    BOOST_CHECK_EQUAL(v_in.size(),dim_in);
    BOOST_CHECK_EQUAL(v_out.size(),dim_out);
    BOOST_CHECK_EQUAL(index_mask.size(),sign_mask.size());

    auto pFT1 = std::make_shared<FeatureTransformSubset>();
    auto pFT2 = std::make_shared<FeatureTransformSubset>();
    auto pFT3 = std::make_shared<FeatureTransformSubset>();
    (*pFT1).set_index_mask(index_mask).set_sign_mask(sign_mask);
    (*pFT2).set_index_mask(index_mask).set_sign_mask(sign_mask);
    (*pFT3).set_index_mask(index_mask);
    BOOST_CHECK_EQUAL(pFT1->dim_in(),dim_in);
    BOOST_CHECK_EQUAL(pFT2->dim_in(),dim_in);
    BOOST_CHECK_EQUAL(pFT3->dim_in(),dim_in);
    BOOST_CHECK_EQUAL(pFT1->dim_out(),dim_out);
    BOOST_CHECK_EQUAL(pFT2->dim_out(),dim_out);
    BOOST_CHECK_EQUAL(pFT3->dim_out(),dim_out);
    BOOST_CHECK_EQUAL(pFT1->get_index_mask(),index_mask);
    BOOST_CHECK_EQUAL(pFT2->get_index_mask(),index_mask);
    BOOST_CHECK_EQUAL(pFT3->get_index_mask(),index_mask);
    BOOST_CHECK_EQUAL(pFT1->get_sign_mask(),sign_mask);
    BOOST_CHECK_EQUAL(pFT2->get_sign_mask(),sign_mask);
    BOOST_CHECK_EQUAL(pFT3->get_sign_mask(),boost::dynamic_bitset<>(dim_in,false));

    pFT1->transform_std_vector(v_in,v_out1);BOOST_CHECK(v_out1==v_out);BOOST_CHECK_EQUAL(v_out1.size(), dim_out);
    pFT2->transform_std_vector(v_in,v_out2);BOOST_CHECK(v_out2==v_out);BOOST_CHECK_EQUAL(v_out2.size(), dim_out);
    pFT3->transform_std_vector(v_in,v_out3);BOOST_CHECK_EQUAL(v_out3.size(), dim_out);

    auto pFV_in = std::make_shared<FeatureVector>(v_in);
    pFV_in->inc_weight_negatives(22);
    pFV_in->inc_weight_positives(33);
    auto pFV_out_truth = std::make_shared<FeatureVector>(v_out);
    const auto pFV_out_real1 = pFT1->transform_feature_vector(pFV_in);
    const auto pFV_out_real2 = pFT2->transform_feature_vector(pFV_in);
    const auto pFV_out_real3 = pFT3->transform_feature_vector(pFV_in);
    BOOST_CHECK_EQUAL(pFV_out_real1->dim(),dim_out);
    BOOST_CHECK_EQUAL(pFV_out_real2->dim(),dim_out);
    BOOST_CHECK_EQUAL(pFV_out_real3->dim(),dim_out);
    BOOST_CHECK_EQUAL(*pFV_out_real1,*pFV_out_truth);
    BOOST_CHECK_EQUAL(*pFV_out_real2,*pFV_out_truth);
    BOOST_CHECK_EQUAL(*pFV_out_real3,FeatureVector({1, 4, 7, 9}));
    BOOST_CHECK_EQUAL(pFV_out_real1->get_weight_negatives(),pFV_in->get_weight_negatives());
    BOOST_CHECK_EQUAL(pFV_out_real2->get_weight_negatives(),pFV_in->get_weight_negatives());
    BOOST_CHECK_EQUAL(pFV_out_real3->get_weight_negatives(),pFV_in->get_weight_negatives());
    BOOST_CHECK_EQUAL(pFV_out_real1->get_weight_positives(),pFV_in->get_weight_positives());
    BOOST_CHECK_EQUAL(pFV_out_real2->get_weight_positives(),pFV_in->get_weight_positives());
    BOOST_CHECK_EQUAL(pFV_out_real3->get_weight_positives(),pFV_in->get_weight_positives());
}

BOOST_AUTO_TEST_CASE( ptree ) {
    std::vector<double> v_in = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<double> v_out = {1, -4, -7, 9}, v_out1, v_out2, v_out3;
    std::string index_mask_str = "01010010010";
    std::string sign_mask_str = "00010010000";
    boost::dynamic_bitset<> index_mask(index_mask_str);
    boost::dynamic_bitset<> sign_mask(sign_mask_str);
    size_t dim_in=index_mask.size();
    size_t dim_out=index_mask.count();

    BOOST_CHECK_EQUAL(v_in.size(),dim_in);
    BOOST_CHECK_EQUAL(v_out.size(),dim_out);
    BOOST_CHECK_EQUAL(index_mask.size(),sign_mask.size());
    auto pFT1_aux = std::make_shared<FeatureTransformSubset>();
    (*pFT1_aux).set_index_mask(index_mask).set_sign_mask(sign_mask);
    boost::property_tree::ptree pt;
    pFT1_aux->dump_to_ptree(pt);
    BOOST_CHECK_EQUAL(pt.size(), 4);
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    BOOST_TEST_MESSAGE("Property tree as json:\n" << ss.str());

    auto pFT1 = std::make_shared<FeatureTransformSubset>(pt);
    BOOST_CHECK_EQUAL(pFT1->dim_in(),dim_in);
    BOOST_CHECK_EQUAL(pFT1->dim_out(),dim_out);
    BOOST_CHECK_EQUAL(pFT1->get_index_mask(),index_mask);
    BOOST_CHECK_EQUAL(pFT1->get_sign_mask(),sign_mask);

    pFT1->transform_std_vector(v_in,v_out1);BOOST_CHECK(v_out1==v_out);BOOST_CHECK_EQUAL(v_out1.size(), dim_out);

    auto pFV_in = std::make_shared<FeatureVector>(v_in);
    pFV_in->inc_weight_negatives(22);
    pFV_in->inc_weight_positives(33);
    auto pFV_out_truth = std::make_shared<FeatureVector>(v_out);
    const auto pFV_out_real1 = pFT1->transform_feature_vector(pFV_in);
    BOOST_CHECK_EQUAL(pFV_out_real1->dim(),dim_out);
    BOOST_CHECK_EQUAL(*pFV_out_real1,*pFV_out_truth);
    BOOST_CHECK_EQUAL(pFV_out_real1->get_weight_negatives(),pFV_in->get_weight_negatives());
    BOOST_CHECK_EQUAL(pFV_out_real1->get_weight_positives(),pFV_in->get_weight_positives());
}

BOOST_AUTO_TEST_SUITE_END()