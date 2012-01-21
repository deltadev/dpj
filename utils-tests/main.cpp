//
//  main.cpp
//  utils-tests
//
//  Created by Daniel James on 12/01/2012.
//  Copyright (c) 2012 deltadev.co.uk. All rights reserved.
//
#include <cmath>
#include <iostream>
#include <vector>

#include "Utils.h"

int main (int argc, const char * argv[])
{

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
            }

            complementNucleotides(inputCopy2.begin(), inputCopy2.end());
            if (inputCopy1 != inputCopy2) 
            {
                std::cerr << "comp test error: " << input << ' ' << inputCopy1 << ' ' << inputCopy2 << '\n';
            }
            std::reverse(inputCopy2.begin(), inputCopy2.end());
            reverseComplementNucleotides(inputCopy3.begin(), inputCopy3.end());
            if (inputCopy2 != inputCopy3)
            {
                std::cerr << "rev comp test error: " << input << ' ' << inputCopy2 << ' ' << inputCopy3 << '\n';
            }
            unsigned hash = nucleotideHash(input.begin(), input.end());
            if (hash != hashTestResult) 
            {
                std::cerr << "hash test error: " << input << ' ' << hash << ' ' << hashTestResult << '\n';
            }
            unsigned revCompHash = nucleotideRevCompHash(input.begin(), input.end());
            unsigned revCompHashCheck = nucleotideHash(inputCopy3.begin(), inputCopy3.end());
            if (revCompHash != revCompHashCheck) 
            {
                std::cerr << "rev comp hash test error: " << input << ' ' << revCompHashCheck << ' ' << revCompHash << '\n';
            }

            std::string inverseHash(inverseNucleotideHash(hashTestResult));
            if (inverseHash != input) {
                std::cerr << "inverse hash test error: " << input << ' ' << hashTestResult << ' ' << inverseHash << '\n'; 
            }
        }
    }


    return 0;
}

