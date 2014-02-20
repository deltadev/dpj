#include <functional>

#include <iostream>
#include <fstream>
#include <sstream>

#include "zstreambuf.hh"

#include <vector>
#include <list>

int main(int argc, const char * argv[])
{
  try {
    
    // Test 1 - test gunzip an entire file.
    //
    {
      std::ifstream f("test_text");
      if (!f.good())
        throw std::runtime_error("couldn't open test_text");
      
      std::string original;
      {
        std::istreambuf_iterator<char> b(f);
        std::istreambuf_iterator<char> e;
        original.assign(b, e);
      }
      
      
      std::ifstream fz("test_text.gz");
      if (!fz.good())
        throw std::runtime_error("couldn't open test_text.gz");
      
      dpj::zstreambuf zbuf(fz.rdbuf());
      std::string gunzipped;
      {
        std::istreambuf_iterator<char> b(&zbuf);
        std::istreambuf_iterator<char> e;
        gunzipped.assign(b, e);
      }
      
      std::cout << "equal outputs: ";
      std::cout << std::boolalpha << (original == gunzipped) << '\n';
    }
    
    
    // Test 2 - streams words and lines, testing equality with original file.
    //
    {
      std::ifstream f("test_text");
      if (!f.good())
        throw std::runtime_error("couldn't open test_text");
      
      std::ifstream fz("test_text.gz");
      if (!fz.good())
        throw std::runtime_error("couldn't open test_text.gz");
      
      dpj::zstreambuf zbuf(fz.rdbuf());
      std::istream zs(&zbuf);
      
      bool all_true = true;
      std::string tok1, tok2;
      for (unsigned i = 0; i < 100; ++i)
      {
        f >> tok1;
        zs >> tok2;
        all_true = all_true && (tok1 == tok2);
      }
      
      std::cout << "all words equal: " << all_true << '\n';
      
      all_true = true;
      for (unsigned i = 0; i < 10; ++i)
      {
        std::getline(f, tok1);
        std::getline(zs, tok2);
        all_true = all_true && (tok1 == tok2);
      }
      
      std::cout << "all lines equal: " << all_true << '\n';
    }
    
    std::stringstream iss;
    
    dpj::zstreambuf_t<16> ozbuf(iss.rdbuf(), std::ios_base::out);
    
    std::ostream os(&ozbuf);
    
    os << "zip this mofo. and this and this and this and this"
          "zip this please. and that and there and here and then.";



    
    os.flush();
    
    std::string s = iss.str();

    
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << '\n';
  }
  return 0;
}