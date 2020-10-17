#define BOOST_TEST_MODULE lib_test_less_relation_iterator
#include <boost/test/included/unit_test.hpp>

#include "sample_creator.h"
#include "less_relation_iterator.h"

BOOST_AUTO_TEST_CASE( less_relation_iterator_vector ) {
    auto pv = std::make_shared<std::vector<int> >(std::vector<int>({22, 11, 44, 33}));
    LessRelationIterator it;
    it.set_container(pv).set_begin();
    std::vector<std::pair<unsigned int,unsigned int> > all_pairs1;
    BOOST_CHECK(*(it) ==(std::pair<unsigned int,unsigned int>(0,2))); all_pairs1.push_back(*it);
    BOOST_CHECK(*(++it) ==(std::pair<unsigned int,unsigned int>(0,3))); all_pairs1.push_back(*it);
    BOOST_CHECK(*(++it) ==(std::pair<unsigned int,unsigned int>(1,0))); all_pairs1.push_back(*it);
    BOOST_CHECK(*(++it) ==(std::pair<unsigned int,unsigned int>(1,2))); all_pairs1.push_back(*it);
    BOOST_CHECK(*(++it) ==(std::pair<unsigned int,unsigned int>(1,3))); all_pairs1.push_back(*it);
    BOOST_CHECK(*(++it) ==(std::pair<unsigned int,unsigned int>(3,2))); all_pairs1.push_back(*it);

    LessRelationIterator it_end; it_end.set_container(pv).set_end();
    BOOST_CHECK(++it == it_end);

    std::vector<std::pair<unsigned int,unsigned int> > all_pairs2;
    for (it.set_begin(); it != it_end; ++it) {
        all_pairs2.push_back(*it);
    }
    BOOST_CHECK(all_pairs1 == all_pairs2);
}

BOOST_AUTO_TEST_CASE( less_relation_iterator_sample ) {
    std::string names_buffer("| this is comment\n"
                             "target_feature.| one more comment\n"
                             "\n"
                             "   feature1: continuous.   \n"
                             "feature2: continuous.   | third comment\n"
                             "feature3: ignore.\n"
                             "feature4: ignore.\n"
                             "target_feature: v1, v2.\n"
                             "case weight: continuous.\n"
                             "feature5: ignore.\n"
                             "\n"
                             "  | one more comment here\n"
                             "\n");
    std::stringstream ss_names((std::stringstream(names_buffer)));
    std::shared_ptr<FeatureNames> pFN = std::make_shared<FeatureNames>(ss_names);
    std::string data_buffer("1,8,34,44,v2,1,77\n"
                             "2,2,34,44,v1,2,77\n"
                             "2,5,34,44,v2,3,77\n"
                             "3,7,34,44,v2,1,77\n"
                             "3,9,34,44,v1,2,77\n"
                             "4,1,34,44,v2,3,77\n"
                             "4,5,34,44,v2,1,77\n"
                             "5,8,34,44,v1,2,77\n"
                             "6,0,34,44,v2,3,77\n"
                             "7,1,34,44,v2,3,77\n"
                             "7,5,34,44,v2,3,77\n"
                             "8,2,34,44,v2,3,77\n"
                             );
    std::stringstream ss_buffer((std::stringstream(data_buffer)));

    SampleCreator sample_creator;
    sample_creator.set_feature_names(pFN);

    std::shared_ptr<Sample> pSample = sample_creator.from_stream(ss_buffer);
    BOOST_TEST_MESSAGE("Sample: " << *pSample);
}
