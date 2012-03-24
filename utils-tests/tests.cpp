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
#include <sstream>
#include <string>


#include "Utils.h"
#include "Utils.hpp"


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


bool orderTest()
{
    bool pass(true);
    int intsArray[7] = {1, 3, 2, -1, 8, 8, 5}; 
    std::vector<int> ints(std::begin(intsArray), std::end(intsArray));
    dpj_utils::Order<std::vector<int>> o(ints);

    dpj_utils::printVector(ints);
    std::cerr << "order is:\n";
    dpj_utils::printVector(o.index());
    dpj_utils::Order<std::vector<unsigned>> rank(o.index());
    std::cerr << "rank is:\n";
    dpj_utils::printVector(rank.index());
    
    return pass;
}

bool histogramTest()
{
    //bool pass(true);
    return false;
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
            std::cout << '\n';
        
        std::cout << f << ' ';
    }
    
    std::cout << '\n';
    return (truth == pwm);
}
