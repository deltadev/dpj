//
//  main.cpp
//  utils-tests
//
//  Created by Daniel James on 12/01/2012.
//  Copyright (c) 2012 deltadev.co.uk. All rights reserved.
//
#include <iostream>

#include "tests.h"
int main (int argc, const char * argv[])
{
  bool result;
  
  std::cout << "running nuc tools test...\n";
  result = nucToolsTest();
  std::cout << "pass: " << std::boolalpha <<  result << std::endl;
  
  std::cout << "running order test...\n";
  result = orderTest();
  std::cout << "pass: " << std::boolalpha <<  result << std::endl;
  
  std::cout << "running readPWM test...\n";
  result = pwmTest();
  std::cout << "pass: " << std::boolalpha <<  result << std::endl;
  
  std::cout << "running histogram test...\n";
  result = histogramTest();
  std::cout << "pass: " << std::boolalpha <<  result << std::endl;
  
  std::cout << "running covariance test...\n";
  result = covariance_test();
  std::cout << "pass: " << std::boolalpha <<  result << std::endl;

  std::cout << "running pearsons test...\n";
  result = pearson_cc_test();
  std::cout << "pass: " << std::boolalpha <<  result << std::endl;

  std::cout << "running spearmans test...\n";
  result = spearman_cc_test();
  std::cout << "pass: " << std::boolalpha <<  result << std::endl;



  return 0;
}

