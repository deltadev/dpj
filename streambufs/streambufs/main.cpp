//
//  main.cpp
//  streambufs
//
//  Created by Daniel James on 12/02/2014.
//  Copyright (c) 2014 Daniel James. All rights reserved.
//

#include <functional>

#include <iostream>
#include <fstream>
#include "zlib_streambuf.hh"

#include <vector>
#include <list>

int main(int argc, const char * argv[])
{
  try {
    
    
    std::ifstream f("dict_of_vulgar_tongue_1811.txt.gz");
    
    
    dpj::zlib_streambuf zbuf(f.rdbuf(), 8);
    std::istream is(&zbuf);
    std::string tok;
    
    
    for (int i = 0; i < 10; ++i)
    {
      is >> tok;
      std::cout << tok << '\n';
    }
    for (int i = 0; i < 10; ++i)
    {
      
      
      std::string line;
      
      int ii = 0;
      
      std::getline(is, line);
      std::cout << line << '\n';
    }
    
    std::cout << std::endl;
    
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << '\n';
  }
  return 0;
}