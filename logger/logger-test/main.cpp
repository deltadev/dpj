//
//  main.cpp
//  logger-test
//
//  Created by Daniel James on 10/10/2012.
//  Copyright (c) 2012 Daniel James. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include "logger.hh"

int main(int argc, const char * argv[])
{
  
  LOG_DEBUG("this should appear in debug only.");
  LOG_DEBUG("this is an error message.");
  
  LOG_INFO(std::setfill(' ') << std::setw(12) << 43 << "more text");

  LOG_INFO("this is an info log message, I'm probably in info.log");
  
  std::cout << Logger::fileName("here/is/a/path/file.cxx") << '\n';

  LOG_DEBUG("does" << " this " << 42 << " work?");
  
  return 0;
}

