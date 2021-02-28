#ifndef TMC_UNIT_TESTS
#define BOOST_TEST_MODULE lib_test_timers
#endif
#include <boost/test/included/unit_test.hpp>

#include "timers.h"

long fibonacci(unsigned n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

BOOST_AUTO_TEST_SUITE( timers )

BOOST_AUTO_TEST_CASE( basics ) {
    auto h = Timers::server().start("fib42");
    fibonacci(3);
    //too slow for unit tests
    //fibonacci(42);
    Timers::server().stop(h);

    auto h_both = Timers::server().start("fib30 both");
    h = Timers::server().start("fib30");
    fibonacci(30);
    Timers::server().stop(h);
    h = Timers::server().start("fib30");
    fibonacci(30);
    Timers::server().stop(h);
    Timers::server().stop(h_both);

    Timers::server().start("it never ends");

    Timers::server().report();
}


BOOST_AUTO_TEST_SUITE_END()