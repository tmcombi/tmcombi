#define BOOST_TEST_MODULE TestRtree25d
#include <boost/test/included/unit_test.hpp>

#include "../../RTree/RTree.h"

#include "../tmcombi_rtree/tmcombi_rtree.h"

#ifndef DIM
#define DIM 25
#endif

#define NUM_BOUNDARY_POINTS 4096
#define NUM_TEST_OBJECTS 500000

double drand() {
    return ((double)rand() / (double)(RAND_MAX));
}

bool leq (const double * const p1, const double * const p2) {
    for(unsigned int i=0; i<DIM; i++) {
        if (p1[i] > p2[i]) return false;
    }
    return true;
}

bool incomparable (const double border[][DIM], const double p[DIM], const unsigned int border_size) {
    for(unsigned int i=0; i<border_size; i++) {
        if (leq(border[i],p))
            return false;
        if (leq(p,border[i]))
            return false;
    }
    return true;
}

bool point_above (const double border[][DIM], const double p[DIM], const unsigned int border_size) {
    for(unsigned int i=0; i<border_size; i++) {
        if (leq(border[i],p))
            return true;
    }
    return false;
}

inline bool MySearchCallback(unsigned int id)
{
    return false;
}

class GenerateBorder {
public:
    GenerateBorder(double p0[DIM], double border[][DIM], unsigned int border_size) {
        for(unsigned int i=0; i<DIM; i++) p0[i]=0;
        std::cout << "Generating border with number of points=" << border_size << std::endl;
        for(unsigned int i=0; i<border_size; i++) {
            do {
                double sum=0;
                for(unsigned int j=0; j<DIM; j++) { border[i][j]=drand(); sum+=border[i][j]; }
                const double squeeze_factor = 1 + 10.87*drand();
                for(unsigned int j=0; j<DIM; j++) border[i][j]=std::min<double>(squeeze_factor*border[i][j]/sum,1);
            } while (!incomparable(border,border[i],i));
        }
        std::cout << "Border created" << std::endl;
    }
};

class GenerateRtree {
public:
    GenerateRtree(RTree<unsigned int,double,DIM> & gh_rtree,
                  TMCombiRTree::RTree<unsigned int,double> & tmcombi_rtree,
                  const double border[][DIM], unsigned int border_size) {
        for(unsigned int i=0; i<border_size; i++) {
            gh_rtree.Insert(border[i],border[i],i);
            tmcombi_rtree.Insert(border[i],border[i],i);
        }
    }
};

class GenerateData2Test {
public:
    GenerateData2Test(double point2test[][DIM], const unsigned int points2test_size) {
        for(unsigned int i=0; i<points2test_size; i++) {
            for(unsigned int j=0; j<DIM; j++) {
                point2test[i][j]=drand();
            }
        }
    }
};

double p0[DIM];
static double border[NUM_BOUNDARY_POINTS][DIM];
static GenerateBorder generateBorder(p0, border, NUM_BOUNDARY_POINTS);

// github rtree: https://github.com/nushoin/RTree.git
// known drawbacks:
// - Unit sphere volumes are not computed for DIM > 20
// - dim is a template parameter
RTree<unsigned int, double, DIM> gh_rtree_1k, gh_rtree_2k, gh_rtree_4k;
TMCombiRTree::RTree<unsigned int, double> tmcombi_rtree_1k(DIM), tmcombi_rtree_2k(DIM), tmcombi_rtree_4k(DIM);

GenerateRtree generateRtree_1k(gh_rtree_1k, tmcombi_rtree_1k, border, NUM_BOUNDARY_POINTS/4);
GenerateRtree generateRtree_2k(gh_rtree_2k, tmcombi_rtree_2k, border, NUM_BOUNDARY_POINTS/2);
GenerateRtree generateRtree_4k(gh_rtree_4k, tmcombi_rtree_4k, border, NUM_BOUNDARY_POINTS);

static double point2test[NUM_TEST_OBJECTS][DIM];
GenerateData2Test generateData2Test(point2test, NUM_TEST_OBJECTS);

BOOST_AUTO_TEST_CASE( test_rtree25d_consistency ) {
    BOOST_CHECK(point_above (border, border[10], NUM_BOUNDARY_POINTS/2) );
    BOOST_CHECK_EQUAL(gh_rtree_2k.Search(p0, border[10], MySearchCallback), 1 );

    unsigned int above_count = 0, below_count=0;
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS/2);
        const bool above_gh = gh_rtree_2k.Search(p0, point2test[i], MySearchCallback) > 0;
        const bool above_tmcombi = tmcombi_rtree_2k.Search(p0, point2test[i], MySearchCallback) > 0;

        if (above_gh && !above_slow) {
            BOOST_FAIL("gh_rtree reports above, but not the point_above");
            exit(-1);
        }
        if (!above_gh && above_slow) {
            BOOST_FAIL("point_above reports true, but not the gh_rtree");
            exit(-1);
        }
        if (above_tmcombi && !above_slow) {
            BOOST_FAIL("tmcombi_rtree reports above, but not the point_above");
            exit(-1);
        }
        if (!above_tmcombi && above_slow) {
            BOOST_FAIL("point_above reports true, but not the tmcombi_rtree");
            exit(-1);
        }
        if (above_slow)
            above_count++;
        else
            below_count++;
    }
    BOOST_TEST_MESSAGE("below_count="	<< below_count << ", above_count=" << above_count);
}

BOOST_AUTO_TEST_CASE( test25d_streight_check_1k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS/4);
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test25d_streight_check_2k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS/2);
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test25d_streight_check_4k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS);
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test25d_gh_rtree_check_1k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_gh = gh_rtree_1k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test25d_gh_rtree_check_2k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_gh = gh_rtree_2k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test25d_gh_rtree_check_4k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_gh = gh_rtree_4k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test25d_tmcombi_rtree_check_1k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_tmcombi = tmcombi_rtree_1k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test25d_tmcombi_rtree_check_2k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_tmcombi = tmcombi_rtree_2k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test25d_tmcombi_rtree_check_4k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_tmcombi = tmcombi_rtree_4k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}