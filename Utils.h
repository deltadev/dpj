//
//  Utils.h
//  HistogramSelector
//
//  Created by Daniel James on 01/01/2012.
//  Copyright (c) 2012 deltadev.co.uk. All rights reserved.
//

#ifndef HistogramSelector_Utils_h
#define HistogramSelector_Utils_h

#include <string>

// Outputs a string of length s.size() that is the nth element of the
// lexicographical ordering of strings from the alphabet {A, C, G, T}.
void unsignedToNucleotides(unsigned n, std::string& s);

unsigned nucleotidesToUnsigned(std::string::iterator b, std::string::iterator e);
unsigned nucleotideHash(std::string::iterator b, std::string::iterator e);
std::string inverseNucleotideHash(unsigned hash);

void complementNucleotides(std::string& s);
void reverseComplementNucleotides(std::string&);


#endif
