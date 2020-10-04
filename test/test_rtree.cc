#define BOOST_TEST_MODULE TestRtree
#include <boost/test/included/unit_test.hpp>
#include "../../RTree/RTree.h"

double drand() {
    return ((double)rand() / (double)(RAND_MAX));
}

struct Point
{
    Point() {
        x = 0;
        y = 0;
        z = 0;
        t = 0;
    }
    Point(double X, double Y, double Z, double T) {
        x = X;
        y = Y;
        z = Z;
        t = T;
    }
    double x;
    double y;
    double z;
    double t;
};

bool leq (const Point &p1, const Point &p2) {
    if (p1.x > p2.x) return false;
    if (p1.y > p2.y) return false;
    if (p1.z > p2.z) return false;
    if (p1.t > p2.t) return false;
    return true;
}

bool incomparable (const Point* const point, const Point &p, unsigned int n) {
    for(unsigned int i=0; i<n; i++) {
        if (leq(point[i],p))
            return false;
        if (leq(p,point[i]))
            return false;
    }
    return true;
}

bool point_above (const Point* const point, const Point &p, unsigned int n) {
    for(unsigned int i=0; i<n; i++) {
        if (leq(point[i],p))
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
    GenerateBorder(Point * border, const unsigned int n) {
        std::cout << "Generating border with number of points=" << n << std::endl;
        for(unsigned int i=0; i<n; i++) {
            do {
                border[i]=Point(drand(), drand(), drand(), drand());
                const double squeeze_factor = 1 + 1.407777778*drand();
                const double sum = border[i].x + border[i].y + border[i].z + border[i].t;
                border[i].x = std::min<double>(squeeze_factor*border[i].x/sum,1);
                border[i].y = std::min<double>(squeeze_factor*border[i].y/sum,1);
                border[i].z = std::min<double>(squeeze_factor*border[i].z/sum,1);
                border[i].t = std::min<double>(squeeze_factor*border[i].t/sum,1);
            } while (!incomparable(border,border[i],i));
        }
        std::cout << "Border created" << std::endl;
    }
};

class GenerateRtree {
public:
    GenerateRtree(RTree<unsigned int,double,4> & tree, Point * border, const unsigned int n) {
        for(unsigned int i=0; i<n; i++) {
            const double p[4]={border[i].x, border[i].y, border[i].z, border[i].t};
            tree.Insert(p,p,i);
        }
    }
};

class GenerateData2Test{
public:
    GenerateData2Test(Point * point2test, double double2test[][4], const unsigned int n) {
        for(unsigned int i=0; i<n; i++) {
            point2test[i] = {drand(), drand(), drand(), drand()};
            double2test[i][0] = point2test[i].x;
            double2test[i][1] = point2test[i].y;
            double2test[i][2] = point2test[i].z;
            double2test[i][3] = point2test[i].t;
        }
    }
};

const double p0[4]={0,0,0,0};
const unsigned int NUM_BOUNDARY_POINTS = 4096;
const unsigned int NUM_TEST_OBJECTS = 500000;
static Point border[NUM_BOUNDARY_POINTS];
static GenerateBorder generateBorder(border, NUM_BOUNDARY_POINTS);
const double p10[4]={border[10].x,border[10].y,border[10].z,border[10].t};

RTree<unsigned int, double, 4> tree_1k, tree_2k, tree_4k;
GenerateRtree generateRtree_1k(tree_1k, border, NUM_BOUNDARY_POINTS/4);
GenerateRtree generateRtree_2k(tree_2k, border, NUM_BOUNDARY_POINTS/2);
GenerateRtree generateRtree_4k(tree_4k, border, NUM_BOUNDARY_POINTS);

static Point point2test[NUM_TEST_OBJECTS];
static double double2test[NUM_TEST_OBJECTS][4];
GenerateData2Test generateData2Test(point2test, double2test, NUM_TEST_OBJECTS);

BOOST_AUTO_TEST_CASE( test_rtree_consistency ) {

    BOOST_CHECK(point_above (border, border[10], NUM_BOUNDARY_POINTS/2) );
    BOOST_CHECK_EQUAL(tree_2k.Search(p0, p10, MySearchCallback), 1 );

    unsigned int above_count = 0, below_count=0;
    BOOST_TEST_MESSAGE("Input objects to test="	<< NUM_TEST_OBJECTS);
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const bool above_slow = point_above (border, point2test[i], NUM_BOUNDARY_POINTS/2);
        const unsigned int nhits = tree_2k.Search(p0, double2test[i], MySearchCallback);
        if (nhits > 0 && !above_slow) {
            BOOST_FAIL("RTree reports above, but not the point_above");
            exit(-1);
        }
        if (nhits == 0 && above_slow) {
            BOOST_FAIL("point_above reports true, but not the RTree");
            exit(-1);
        }
        if (above_slow)
            above_count++;
        else
            below_count++;
    }
    BOOST_TEST_MESSAGE("below_count="	<< below_count << ", above_count=" << above_count);
}
