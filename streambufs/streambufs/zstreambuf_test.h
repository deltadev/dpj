#ifndef _ZSTREAMBUF_TEST_H_
#define _ZSTREAMBUF_TEST_H_

#include <functional>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <iterator>

#include "zstreambuf.hh"

#include <vector>
#include <list>


struct test_data
{
  uint32_t a = 42;                                   // 4
  std::string b = "foobar";                          // 6
  std::array<uint8_t, 6> c{{31,63,127,143,159,255}}; // 6  aggregate initialisation
};

std::ostream& operator<<(std::ostream& os, test_data const& d);
std::istream& operator>>(std::istream& is, test_data& d);
bool operator==(test_data const& d1, test_data const& d2);

bool test_decompress_gzip_file(std::string zfname, std::string fname);
bool test_decompress_read_tokens(std::string zfname, std::string fname);
bool test_aifstream_read_tokens(std::string zfname, std::string fname);

bool test_decompress_objects(std::string ref, unsigned n);
bool test_compress_objects();
bool test_deflate_zlib_string(std::string const& str, std::vector<uint8_t> const& bytes);
void write_objects_and_compress(std::string name,
                                std::vector<test_data> const& objs, bool gzip = true);
bool test_compress_gzip_file(std::string ref, std::string target);



#endif /* _ZSTREAMBUF_TEST_H_ */
