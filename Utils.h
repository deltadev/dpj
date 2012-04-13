//
//  Utils.h
//  DPJUtils
//
//  Created by Daniel James on 01/01/2012.
//  Copyright (c) 2012 deltadev.co.uk. All rights reserved.
//

#ifndef DPJUtils_Utils_h
#define DPJUtils_Utils_h

#include <string>
#include <vector>
#include <iostream>

// Outputs a string of length s.size() that is the nth element of the
// lexicographical ordering of strings from the alphabet {A, C, G, T}.
void unsignedToNucleotides(unsigned n, std::string& s);

unsigned nucleotidesToUnsigned(std::string::const_iterator b, std::string::const_iterator e);


// hash nucleotide nmers of various lengths to unique buckets.
unsigned nucleotideHash(std::string::const_iterator b, std::string::const_iterator e);
unsigned nucleotideRevCompHash(std::string::const_iterator b, std::string::const_iterator e);
std::string inverseNucleotideHash(unsigned hash);

void complementNucleotides(std::string::iterator b, std::string::iterator e);
void reverseComplementNucleotides(std::string::iterator b, std::string::iterator e);



std::string readFile(std::string fileName);

std::vector<float> readPWM(std::istream& is, bool rowMajorStorage = false);

std::string readFasta(std::string fileName);

#endif
