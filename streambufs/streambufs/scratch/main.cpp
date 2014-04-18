#include <iostream>
#include <sstream>

#include <string>
int main(int argc, const char * argv[])
{
  std::stringstream ss{"Hello, World!\n"};
  
  std::string tok;
  ss >> tok;
  auto rdstate = ss.rdstate(); // 0

  ss >> tok;
  rdstate = ss.rdstate();      // 0

  ss >> std::ws;               // sets eofbit
  rdstate = ss.rdstate();      // 2
  
  ss.clear();
  rdstate = ss.rdstate();      // 0
    
  ss >> std::ws; // sets eofbit
  rdstate = ss.rdstate(); // 2
  
  ss >> std::ws; // sets failbit
  rdstate = ss.rdstate(); // 6
  
  
  
  
  return 0;
}

