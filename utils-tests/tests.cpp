//
//  tests.cpp
//  dpj-utils
//
//  Created by Daniel James on 06/02/2012.
//  Copyright (c) 2012 deltadev.co.uk. All rights reserved.
//
#include "tests.h"


#include <cmath>

#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <string>

#include "prettyprint.hpp"
#include "histogram.hh"

#include "dpj_utils.h"
#include "dpj_utils.hh"


bool nucToolsTest()
{
  bool pass = true;
  
  //
  // This is for testing the hash:
  //     A -> 0, C-> 1, ..., T-> 3,
  //     AA -> 4, AC -> 5, ..., TT-> 19,
  //     AAA -> 20, ...
  //
  unsigned hashTestResult = 0;
  
  
  for (unsigned k = 1; k < 4; ++k)
  {
    //
    // We test all DNA strings of length k.
    //
    std::string input(k, ' ');
    for (unsigned l = 0; l < ::pow(4, k); ++l, ++hashTestResult)
    {
      unsignedToNucleotides(l, input);
      
      std::string inputCopy1(input);
      std::string inputCopy2(input);
      std::string inputCopy3(input);
      
      unsignedToNucleotides(::pow(4, k) - l - 1, inputCopy1);
      std::cerr << "nucs: " << input << ' ' << " comp: " << inputCopy1 << '\n';
      
      if (nucleotidesToUnsigned(input.begin(), input.end()) != l)
      {
        std::cerr << "string_to_unsigned test error: " << input << ' ' << l << '\n';
        pass = false;
      }
      
      complementNucleotides(inputCopy2.begin(), inputCopy2.end());
      if (inputCopy1 != inputCopy2)
      {
        std::cerr << "comp test error: " << input << ' ' << inputCopy1 << ' ' << inputCopy2 << '\n';
        pass = false;
      }
      std::reverse(inputCopy2.begin(), inputCopy2.end());
      reverseComplementNucleotides(inputCopy3.begin(), inputCopy3.end());
      if (inputCopy2 != inputCopy3)
      {
        std::cerr << "rev comp test error: " << input << ' ' << inputCopy2 << ' ' << inputCopy3 << '\n';
        pass = false;
      }
      unsigned hash = nucleotideHash(input.begin(), input.end());
      if (hash != hashTestResult)
      {
        std::cerr << "hash test error: " << input << ' ' << hash << ' ' << hashTestResult << '\n';
        pass = false;
      }
      unsigned revCompHash = nucleotideRevCompHash(input.begin(), input.end());
      unsigned revCompHashCheck = nucleotideHash(inputCopy3.begin(), inputCopy3.end());
      if (revCompHash != revCompHashCheck)
      {
        std::cerr << "rev comp hash test error: " << input << ' ' << revCompHashCheck << ' ' << revCompHash << '\n';
        pass = false;
      }
      
      std::string inverseHash(inverseNucleotideHash(hashTestResult));
      if (inverseHash != input)
      {
        std::cerr << "inverse hash test error: " << input << ' ' << hashTestResult << ' ' << inverseHash << '\n';
        pass = false;
      }
    }
  }
  
  return pass;
}

using std::cout;
using std::cerr;
bool orderTest()
{
  bool pass(true);
  int intsArray[7] = {1, 3, 2, -1, 8, 8, 5};
  cout << "data is:\n" << intsArray << '\n';
  
  std::vector<int> ints(std::begin(intsArray), std::end(intsArray));
  std::vector<int> ordered(ints.size());
  dpj::order(ints.begin(), ints.end(), ordered.begin());
  cout << "order is:\n" << ordered << '\n';
  
  dpj::order(ordered.begin(), ordered.end(), ordered.begin());
  cout << "rank is:\n" << ordered << '\n';
  
  return pass;
}

bool histogramTest()
{
  std::vector<double> rs{0, 1,2,3,4,5.0000000};
  Histogram<double> h(rs.begin(), rs.end());
  h.binData();
  h.draw();
  h.summary();
  return true;
}

bool pwmTest()
{
  std::istringstream ss;
  ss.str("1 2 3 4 5 6 7 8");
  std::vector<float> pwm = readPWM(ss, false);
  unsigned counter = 0;
  float truthArray[8] = {1, 5, 2, 6, 3, 7, 4, 8};
  std::vector<float> truth(truthArray, truthArray + 8);
  for (float f : pwm)
  {
    if ( ! (counter++ % (pwm.size()/4)))
      cout << '\n';
    
    cout << f << ' ';
  }
  
  cout << '\n';
  return (truth == pwm);
}

std::array<double, 25> x{
  {
    0.95066605,  0.73844443,  0.25084646,  0.48559874,  0.7162443 ,
    0.93823069,  0.00725236,  0.43974925,  0.63150104,  0.8168913 ,
    0.20459832,  0.99897657,  0.60544523,  0.50133834,  0.79542532,
    0.32799355,  0.07951997,  0.70075721,  0.8468032 ,  0.08510824,
    0.64527051,  0.37971715,  0.61769385,  0.03528504,  0.41282303
  }
};
std::array<double, 25> y{{   
    0.16297828, 0.48221679, 0.84467651, 0.84281474, 0.08119215,
    0.77225741, 0.43086105, 0.15751603, 0.02472734, 0.10697636,
    0.98900186, 0.20101628, 0.11519468, 0.58241635, 0.43585047,
    0.60467086, 0.99328387, 0.99207509, 0.31556858, 0.2553983 ,
    0.51795227, 0.00341642, 0.17557971, 0.8360058 , 0.98655175}
};

bool covariance_test() {
  double res = dpj::covariance(x.begin(), x.end(), y.begin());
  double actual = -0.03959601;
  std::cout << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}

bool pearson_cc_test() {
  double res = dpj::pearsons_cc(x.begin(), x.end(), y.begin());
  double actual = -0.38256335;
  std::cout << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}

bool spearman_cc_test() {
  double res = dpj::spearmans_cc(x.begin(), x.end(), y.begin());
  double actual = -0.34923076923076929;
  std::cout << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}














