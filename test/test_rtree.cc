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

BOOST_AUTO_TEST_CASE( test_rtree_consistency ) {
    const int NUM_BOUNDARY_POINTS = 4096;
    const int NUM_TEST_OBJECTS = 500000;

    std::cout << "NUM_BOUNDARY_POINTS="	<< NUM_BOUNDARY_POINTS << std::endl;

    Point border[NUM_BOUNDARY_POINTS];
    for(unsigned int i=0; i<NUM_BOUNDARY_POINTS; i++) {
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
    std::cout << "boundary created" << std::endl;
    RTree<unsigned int, double, 4> tree;
    for(unsigned int i=0; i<NUM_BOUNDARY_POINTS/2; i++) {
        const double p[4]={border[i].x, border[i].y, border[i].z, border[i].t};
        tree.Insert(p,p,i);
    }

    unsigned int above_count = 0, below_count=0;
    const double p0[4]={0,0,0,0};

    //small test
    const double p10[4]={border[10].x,border[10].y,border[10].z,border[10].t};
    BOOST_CHECK(point_above (border, border[10], NUM_BOUNDARY_POINTS/2) );
    BOOST_CHECK_EQUAL(tree.Search(p0, p10, MySearchCallback), 1 );

    std::cout << "NUM_TEST_OBJECTS="	<< NUM_TEST_OBJECTS << std::endl;
    for(unsigned int i=0; i<NUM_TEST_OBJECTS; i++) {
        const Point p(drand(), drand(), drand(), drand());
        const double pd[4]={p.x,p.y,p.z,p.t};
        const bool above_slow = point_above (border, p, NUM_BOUNDARY_POINTS/2);
        const unsigned int nhits = tree.Search(p0, pd, MySearchCallback);
        if (nhits > 0 && !above_slow)
            BOOST_FAIL("RTree reports above, but not the point_above");
        if (nhits == 0 && above_slow)
            BOOST_FAIL("point_above reports true, but not the RTree");
        if (above_slow)
            above_count++;
        else
            below_count++;
    }
    std::cout << "below_count="	<< below_count << ", above_count=" << above_count << std::endl;

}
