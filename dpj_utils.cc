//
//  Utils.cpp
//  DPJUtils
//
//  Created by Daniel James on 01/01/2012.
//  Copyright (c) 2012 deltadev.co.uk. All rights reserved.
//

#include <iostream>
#include <fstream>

#include <algorithm>
#include <numeric>
#include <cmath>

#include "dpj_utils.h"

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
      {
        std::cerr << "encountered symbol not in alphabet: " << nextSymbol << '\n';
        exit(EXIT_FAILURE);
      }
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
        else return 'N';
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


std::string readFile(std::string fileName)
{
  std::ifstream t(fileName.c_str());
  if (! t.good()) 
  {
    std::cerr << "Problem opening source file: " << fileName << ".\n";
  }
    
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  std::string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size); 
    
  if (t.fail()) 
  {
    std::cerr << "Failed to read from file: " << fileName << ". Exiting...\n";
    exit(EXIT_FAILURE);
  }
  return buffer;
}

//
// The PWM should have the natural layout:
// 
//   col1 col2 ... colN
// A m11  m12  ... m1n
// C m21
// G m31
// T m41       ... m4n
//
// or its transpose.
// 
std::vector<float> readPWM(std::istream& is, bool rowMajorStorage)
{
  float tok;    
  std::vector<float> tmp;
  while (is >> tok)
  {
    tmp.push_back(tok);
  }
  std::vector<float> pwm(tmp);

  if ( ! rowMajorStorage)
  {
    // transpose.
    unsigned motifLength = (unsigned)tmp.size()/4;
    unsigned counter = 0;
    for (auto it = tmp.begin(); it != tmp.end(); ++it)
    {            
      unsigned row = counter % 4;
      unsigned col = counter / 4;
      pwm[row * motifLength + col] = *it;
      counter++;
    }
  }
  return pwm;
}


std::string readFasta(std::string fileName)
{
  std::string fasta, line;
  std::ifstream iFile(fileName.c_str());
  std::getline(iFile, line); // header
  unsigned lineCounter = 0;
  while (iFile >> line)
  {
    lineCounter++;
    fasta += line;
  }
  std::cerr << "read " << lineCounter << " lines of fasta.\n";
    
  return fasta;
}

namespace dpj {
  std::string strip_string(std::string s) {
    auto fnb = s.find_first_not_of(" \r\n\t");
    auto lnb = s.find_last_not_of(" \r\n\t");
    return s.substr(fnb, lnb - fnb + 1);
  }
}
