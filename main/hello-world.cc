#include <boost/lambda/lambda.hpp>
#include "lib/sample.h"
#include "main/hello-greet.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  using namespace boost::lambda;
  
  std::string who = "world";
  if (argc > 1) {
    who = argv[1];
  }
  std::cout << get_greet(who) << std::endl;
  print_localtime();
  return 0;
}
