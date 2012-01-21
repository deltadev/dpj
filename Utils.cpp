//
//  Utils.cpp
//  HistogramSelector
//
//  Created by Daniel James on 01/01/2012.
//  Copyright (c) 2012 deltadev.co.uk. All rights reserved.
//

#include <iostream>

#include <algorithm>
#include <numeric>
#include <cmath>

#include "Utils.h"

void unsignedToNucleotides(unsigned n, std::string& s)
{
    char nucs[] = "ACGT";
    size_t l = s.size();
    for(unsigned pos = 0; pos < l; ++pos)
    {
        unsigned mask = 3 << (2 * pos);
        char nuc = nucs[(n & mask) >> (2 * pos)];
        s[l - pos - 1] = nuc;
    }
}

struct NucleotidesToBits : std::binary_function<unsigned, char, unsigned>
{
    unsigned operator()(unsigned currentBits, char nextSymbol) 
    {
        currentBits <<= 2;
        unsigned bits = 0;
        if (nextSymbol == 'A' || nextSymbol == 'a') 
            bits = 0;
        else if (nextSymbol == 'C' || nextSymbol == 'c') 
            bits = 1;
        else if (nextSymbol == 'G' || nextSymbol == 'g') 
            bits = 2;
        else if (nextSymbol == 'T' || nextSymbol == 't') 
            bits = 3;
        else 
            exit(EXIT_FAILURE);
        return currentBits | bits;
    }
};
struct NucleotidesToCompBits : std::binary_function<unsigned, char, unsigned>
{
    unsigned operator()(unsigned currentBits, char nextSymbol) 
    {
        currentBits <<= 2;
        unsigned bits = 0;
        if (nextSymbol == 'A' || nextSymbol == 'a') 
            bits = 3;
        else if (nextSymbol == 'C' || nextSymbol == 'c') 
            bits = 2;
        else if (nextSymbol == 'G' || nextSymbol == 'g') 
            bits = 1;
        else if (nextSymbol == 'T' || nextSymbol == 't') 
            bits = 0;
        else 
            exit(EXIT_FAILURE);
        return currentBits | bits;
    }
};

unsigned nucleotidesToUnsigned(std::string::const_iterator b, std::string::const_iterator e)
{
    return std::accumulate(b, e, 0, NucleotidesToBits());
}

unsigned nucleotideHash(std::string::const_iterator b, std::string::const_iterator e)
{
    unsigned hash = std::accumulate(b, e, 0, NucleotidesToBits());
    return hash + 4 * (::pow(4, std::distance(b, e) - 1) - 1) / 3;
}

unsigned nucleotideRevCompHash(std::string::const_iterator b, std::string::const_iterator e)
{
    unsigned hash = std::accumulate(b, e, 0, NucleotidesToCompBits());
    
    // Now we need to rotate the bits...
    size_t len = std::distance(b, e);
    unsigned newHash = 0;
    for (unsigned i = 0; i < len; ++i) 
    {
        newHash <<= 2;
        newHash |= hash & 3;
        hash >>= 2;
    }
    return newHash + 4 * (::pow(4, len - 1) - 1) / 3;
}

std::string inverseNucleotideHash(unsigned hash)
{
    unsigned l = 1;
    unsigned q = 4;
    hash = 3 * hash + 4;
    while (hash / q) 
    {
        q *= 4;
        l++;
    }
    std::string unHash(l - 1, ' ');
    unsignedToNucleotides((hash - q / 4) / 3, unHash);
    return unHash;
}


namespace {
    struct Complement
    {
        char operator()(char c)
        {
            if (c == 'A' || c == 'a') return 'T';
            else if (c == 'C' || c == 'c') return 'G';
            else if (c == 'G' || c == 'g') return 'C';
            else if (c == 'T' || c == 't') return 'A';
            else return 'Z';
        }
    };
}
void complementNucleotides(std::string::iterator b, std::string::iterator e)
{
    std::transform(b, e, b, Complement());
}
void reverseComplementNucleotides(std::string::iterator b, std::string::iterator e)
{
    std::reverse(b, e);
    std::transform(b, e, b, Complement());
}

