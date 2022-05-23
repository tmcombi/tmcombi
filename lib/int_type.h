#ifndef LIB_INT_TYPE_H_
#define LIB_INT_TYPE_H_

#include <boost/multiprecision/cpp_int.hpp>

//typedef int IntType;
//typedef long int IntType;
//typedef long long int IntType;
typedef boost::multiprecision::int128_t IntType;
//typedef boost::multiprecision::cpp_int IntType; //<--- it does not work properly, max-flow?

// seems to be ok for long int
#define PRECISION 100000

#endif
