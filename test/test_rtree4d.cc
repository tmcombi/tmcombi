#define BOOST_TEST_MODULE TestRtree4d
#include <boost/test/included/unit_test.hpp>

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "../../RTree/RTree.h"
#ifdef RTREE_H
#undef RTREE_H
#endif

#include "../../DynDimRTree/RTree.h"

#ifndef DIM
#define DIM 4
#endif
#ifndef STRATEGY
#define STRATEGY quadratic<16>
#endif
#define NUM_BOUNDARY_POINTS 4096
#define NUM_TEST_OBJECTS 500000

typedef boost::geometry::model::point<double, DIM, boost::geometry::cs::cartesian> bg_point;
typedef boost::geometry::model::box<bg_point> bg_box;
typedef std::pair<bg_box, unsigned int> bg_value;

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
                const double squeeze_factor = 1 + 1.407777778*drand();
                for(unsigned int j=0; j<DIM; j++) border[i][j]=std::min<double>(squeeze_factor*border[i][j]/sum,1);
            } while (!incomparable(border,border[i],i));
        }
        std::cout << "Border created" << std::endl;
    }
};

class GenerateRtree {
public:
    GenerateRtree(RTree<unsigned int,double,DIM> & gh_rtree,
                  DynDimRTree::RTree<unsigned int,double> & dyndim_rtree,
                  boost::geometry::index::rtree< bg_value, boost::geometry::index::STRATEGY > & bg_rtree,
                  const double border[][DIM], unsigned int border_size) {
        for(unsigned int i=0; i<border_size; i++) {
            gh_rtree.Insert(border[i],border[i],i);
            dyndim_rtree.Insert(border[i],border[i],i);

            bg_point bgp;
            auto bgp_data = &bgp.get<0>();
            memcpy((double *)bgp_data, border[i], DIM*sizeof(double)); //ugly cast
            const bg_box b(bgp,bgp);
            bg_rtree.insert(std::make_pair(b,i));
        }
    }
};

class GenerateData2Test {
public:
    GenerateData2Test(double point2test[][DIM],
                      boost::geometry::index::detail::predicates::spatial_predicate
                              <bg_box, boost::geometry::index::detail::predicates::intersects_tag, false> bg_predicate[],
                              const unsigned int points2test_size) {
        bg_point bg_point0;
        auto bg_point0_data = &bg_point0.get<0>();
        memset((double *)bg_point0_data, 0, DIM*sizeof(double)); //ugly cast
        for(unsigned int i=0; i<points2test_size; i++) {
            for(unsigned int j=0; j<DIM; j++) {
                point2test[i][j]=drand();
            }
            bg_point bgp;
            auto bgp_data = &bgp.get<0>();
            memcpy((double *)bgp_data, point2test[i], DIM*sizeof(double)); //ugly cast
            bg_predicate[i] = boost::geometry::index::intersects(bg_box(bg_point0,bgp));
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
DynDimRTree::RTree<unsigned int, double> dyndim_rtree_1k(DIM), dyndim_rtree_2k(DIM), dyndim_rtree_4k(DIM);

// boost rtree
// known drawbacks: slow for my use case?
boost::geometry::index::rtree< bg_value, boost::geometry::index::STRATEGY > bg_rtree_1k, bg_rtree_2k, bg_rtree_4k;
static auto bg_rtree_1k_qend = bg_rtree_1k.qend();
static auto bg_rtree_2k_qend = bg_rtree_2k.qend();
static auto bg_rtree_4k_qend = bg_rtree_4k.qend();

GenerateRtree generateRtree_1k(gh_rtree_1k, dyndim_rtree_1k, bg_rtree_1k, border, NUM_BOUNDARY_POINTS/4);
GenerateRtree generateRtree_2k(gh_rtree_2k, dyndim_rtree_2k, bg_rtree_2k, border, NUM_BOUNDARY_POINTS/2);
GenerateRtree generateRtree_4k(gh_rtree_4k, dyndim_rtree_4k, bg_rtree_4k, border, NUM_BOUNDARY_POINTS);

static double point2test[NUM_TEST_OBJECTS][DIM];
static boost::geometry::index::detail::predicates::spatial_predicate
        <bg_box, boost::geometry::index::detail::predicates::intersects_tag, false> bg_predicate[NUM_TEST_OBJECTS];
GenerateData2Test generateData2Test(point2test, bg_predicate, NUM_TEST_OBJECTS);

BOOST_AUTO_TEST_CASE( test_rtree4d_consistency ) {
    BOOST_CHECK(point_above (border, border[10], NUM_BOUNDARY_POINTS/2) );
    BOOST_CHECK_EQUAL(gh_rtree_2k.Search(p0, border[10], MySearchCallback), 1 );

    unsigned int above_count = 0, below_count=0;
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS/2);
        const bool above_gh = gh_rtree_2k.Search(p0, point2test[i], MySearchCallback) > 0;
        const bool above_tmcombi = dyndim_rtree_2k.Search(p0, point2test[i], MySearchCallback) > 0;
        const bool above_bg = bg_rtree_2k.qbegin(bg_predicate[i]) != bg_rtree_2k_qend;

        if (above_gh && !above_slow) {
            BOOST_FAIL("gh_rtree reports above, but not the point_above");
            exit(-1);
        }
        if (!above_gh && above_slow) {
            BOOST_FAIL("point_above reports true, but not the gh_rtree");
            exit(-1);
        }
        if (above_tmcombi && !above_slow) {
            BOOST_FAIL("dyndim_rtree reports above, but not the point_above");
            exit(-1);
        }
        if (!above_tmcombi && above_slow) {
            BOOST_FAIL("point_above reports true, but not the dyndim_rtree");
            exit(-1);
        }
        if (above_bg && !above_slow) {
            BOOST_FAIL("bg_rtree reports above, but not the point_above");
            exit(-1);
        }
        if (!above_bg && above_slow) {
            BOOST_FAIL("point_above reports true, but not the bg_rtree");
            exit(-1);
        }
        if (above_slow)
            above_count++;
        else
            below_count++;
    }
    BOOST_TEST_MESSAGE("below_count="	<< below_count << ", above_count=" << above_count);
}

BOOST_AUTO_TEST_CASE( test4d_streight_check_1k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS/4);
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_streight_check_2k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS/2);
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_streight_check_4k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS);
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_gh_rtree_check_1k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_gh = gh_rtree_1k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_gh_rtree_check_2k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_gh = gh_rtree_2k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_gh_rtree_check_4k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_gh = gh_rtree_4k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_dyndim_rtree_check_1k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_tmcombi = dyndim_rtree_1k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_dyndim_rtree_check_2k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_tmcombi = dyndim_rtree_2k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_dyndim_rtree_check_4k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_tmcombi = dyndim_rtree_4k.Search(p0, point2test[i], MySearchCallback) > 0;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_bg_rtree_check_1k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_bg = bg_rtree_1k.qbegin(bg_predicate[i]) != bg_rtree_1k_qend;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_bg_rtree_check_2k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_bg = bg_rtree_2k.qbegin(bg_predicate[i]) != bg_rtree_2k_qend;
    }
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE( test4d_bg_rtree_check_4k ) {
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_bg = bg_rtree_4k.qbegin(bg_predicate[i]) != bg_rtree_4k_qend;
    }
    BOOST_CHECK(true);
}
