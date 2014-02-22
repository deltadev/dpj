#include <functional>

#include <iostream>
#include <fstream>
#include <sstream>

#include <iterator>

#include "zstreambuf.hh"

#include <vector>
#include <list>

#include "hex_b64_convert.h"





/// tests inflating a a block of zlib bytes.
//
bool test_inflate_zlib_bytes(std::vector<uint8_t> const& bytes, std::string& truth)
{
  
  return true;
}

typedef std::istreambuf_iterator<char> isb_it;

/// tests inflating a zlib compressed file.
//
bool test_decompress_gzip_file(std::string zfname, std::string fname)
{

  dpj::zifstream zifs{zfname}; // decompressor
  std::ifstream  ifs{fname};
  
  std::string zifs_str{isb_it{zifs}, isb_it{}};
  std::string ifs_str{isb_it{ifs}, isb_it{}};

  zifs.close();
  ifs.close();
  
  return ifs_str == zifs_str;
}

/// tests deflating a file.
//
bool test_compress_gzip_file(std::string ref, std::string target)
{
  // Temporary file to receive our compression output.
  //
  {
    dpj::zofstream zof{target+".gz"};
    if (!zof.good())
      throw std::runtime_error{"couldn't open file: "+target+".gz for writing."};
    zof.rdbuf()->write_gzip_header(target, 3, false);
    std::ifstream ifs{target};
    if (!ifs.good())
      throw std::runtime_error{"couldn't open file: "+target+" for reading."};
    zof << ifs.rdbuf();
    zof.flush();
  }


  // Open our reference compressed file.
  // This file was made by:
  // $ cp test_text zof_test_out && gzip zof_test_out && mv zof_test_out.gz zof_test_ref.gz
  //
  std::ifstream ifs_z{ref};
  if (!ifs_z.good())
    throw std::runtime_error{"couldn't open file: "+ref+".gz for reading."};
  std::vector<uint8_t> ref_bytes{isb_it{ifs_z}, isb_it{}};
  
  // Reads in the file we wrote above.
  //
  std::ifstream zof_z{target+".gz"};
  if (!zof_z.good())
    throw std::runtime_error{"couldn't open file: "+target+".gz for reading."};

  std::vector<uint8_t> target_bytes{isb_it{zof_z}, isb_it{}};

  
  // We'll set the unix time stamp the same.
  //
  target_bytes[4] = ref_bytes[4];
  target_bytes[5] = ref_bytes[5];
  target_bytes[6] = ref_bytes[6];
  target_bytes[7] = ref_bytes[7];
  
  return ref_bytes == target_bytes;
}

/// tests deflating a string into zlib block of bytes.
//
bool test_deflate_zlib_string(std::string const& str, std::vector<uint8_t> const& bytes)
{
  std::string byte_str{bytes.begin(), bytes.end()};
  std::istringstream iss{byte_str};
  
  dpj::zstreambuf zsbuf{iss.rdbuf(), dpj::zstreambuf::action::comp_zlib};
  
  std::istreambuf_iterator<char> b{&zsbuf};
  std::istreambuf_iterator<char> e;
  
  return str == std::string(b, e);
}

int main(int argc, const char * argv[])
{
  try {
    
    // Test 1 - test gunzip an entire file.
    //
    {
      std::cout << "test_decompress_zlib_file: ";
      std::cout << std::boolalpha << test_decompress_gzip_file("test_text.gz", "test_text") << '\n';

      std::cout << "test_compress_zlib_file: ";
      std::cout << std::boolalpha << test_compress_gzip_file("zof_ref.gz", "zof_target") << '\n';
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
      
      dpj::zstreambuf zbuf{fz.rdbuf(), dpj::zstreambuf::action::decomp_zlib};
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
    
    {
      // iss provides the output streambuf.
      std::stringstream iss;

      // this is our filtering zstreambuf
      dpj::zstreambuf ozbuf{iss.rdbuf(), dpj::zstreambuf::action::comp_zlib};

      // provides an ostream interface for the filter.
      std::ostream os{&ozbuf};

      std::string data{"zip this mofo. and this and this and that."};
      os << data;
      os.flush();
      
      // oss provides an input streambuf.
      std::stringstream oss{iss.str()};
      
      // this is our filter.
      dpj::zstreambuf izbuf{oss.rdbuf(), dpj::zstreambuf::action::decomp_zlib};

      // provides an istream interface.
      std::istream is{&izbuf};
      
      std::istreambuf_iterator<char> b(is);
      std::istreambuf_iterator<char> e;
      std::string tok{b, e};

      std::cout << "data == tok: " << std::boolalpha << (data == tok) << '\n';
      
    }
    
    std::string txt_str;
    std::string txt_gz_str;
    {
      std::ifstream txt{"test_text"};
      std::istreambuf_iterator<char> b{txt};
      std::istreambuf_iterator<char> e;
      txt_str.assign(b, e);
    }
    {
      std::ifstream txt_gz{"test_text.gz"};
      std::istreambuf_iterator<char> b{txt_gz};
      std::istreambuf_iterator<char> e;
      txt_gz_str.assign(b, e);
    }
    
    std::ostringstream oss;
    dpj::zstreambuf ozs(oss.rdbuf(), dpj::zstreambuf::action::comp_zlib);
    ozs.sputn(txt_str.data(), txt_str.size());
    ozs.pubsync();

    std::string str{oss.str()};

    std::cout << "\nfirst bytes:\n";
    // Look at first bytes minus the headers.
    //
    std::vector<uint8_t> cmpd{str.data(), str.data()+str.size()};
    std::cout << "size of zstreambuf: " << cmpd.size() << '\n';
    auto hex = bytes_to_hex(cmpd.begin(), cmpd.end());
    std::cout << hex.substr(0, 100) << '\n';

    cmpd.assign(txt_gz_str.data(), txt_gz_str.data() + txt_gz_str.size());
    std::cout << "size of gzipped: " << cmpd.size() << '\n';
    auto hex2 = bytes_to_hex(cmpd.begin(), cmpd.end());
    std::cout << hex2.substr(0, 100)<< '\n';
    
    std::cout << "first bytes, (minus header), equal: " << std::boolalpha
              << (hex.substr(4, 10) == hex2.substr(40, 10)) << '\n';
    
    std::cout << "\n\nlast bytes:\n";
    // Look at final bytes.
    //
    cmpd.assign(str.data(), str.data() + str.size());
    hex = bytes_to_hex(cmpd.begin(), cmpd.end());
    std::cout << hex.substr(hex.size() - 100) << '\n';
    hex = hex.substr(hex.size() - 108, 100);

    cmpd.assign(txt_gz_str.data(), txt_gz_str.data() + txt_gz_str.size());
    hex2 = bytes_to_hex(cmpd.begin(), cmpd.end());
    std::cout << hex2.substr(hex2.size() - 100) << '\n';
    hex2 = hex2.substr(hex2.size() - 116, 100);

    std::cout << "last bytes, (minus trailer), equal: "
              << std::boolalpha << (hex == hex2) << '\n';
    
    
    
    
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << '\n';
  }
  return 0;
}