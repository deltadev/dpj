#include "zstreambuf_test.h"

typedef std::istreambuf_iterator<char> isb_it;

std::ostream& operator<<(std::ostream& os, test_data const& d)
{
  os.write((char*)&(d.a), 4);
  os.write((char*)d.b.data(), 6);
  os.write((char*)&(d.c), 6);
  return os;
};
std::istream& operator>>(std::istream& is, test_data& d)
{
  is.read((char*)&(d.a), 4);
  std::array<char, 6> str;
  is.read(str.data(), 6);
  d.b.assign(str.begin(), str.end());
  is.read((char*)&(d.c), 6);
  return is;
};
bool operator==(test_data const& d1, test_data const& d2)
{ return d1.a == d2.a && d1.b == d2.b && d1.c == d2.c; }
std::vector<test_data> random_objects(unsigned n)
{
  std::vector<test_data> objects{n};
  srandom(0);
  std::generate(objects.begin(), objects.end(),
                []() { test_data td; td.a = (uint32_t)random(); return td; });
  return objects;
}
bool compare_files(std::string f1, std::string f2)
{
  std::ifstream if1{f1};
  if (!if1.good())
    throw std::runtime_error("compare_files: couldn't open "+f1);
  std::vector<uint8_t> bytes1{isb_it{if1}, isb_it{}};
  
  std::ifstream if2{f2};
  if (!if2.good())
    throw std::runtime_error("compare_files: couldn't open "+f2);
  std::vector<uint8_t> bytes2{isb_it{if2}, isb_it{}};
  
  if (bytes1[0] == 0x1f && bytes1[1] == 0x8b)
  {
    // We might have two gzip files, so equalise timestamps to give them a chance of
    // comparing equal.
    //
    bytes2[4] = bytes1[4];
    bytes2[5] = bytes1[5];
    bytes2[6] = bytes1[6];
    bytes2[7] = bytes1[7];
  }
  return bytes1 == bytes2;
}

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
bool test_decompress_read_tokens(std::string zfname, std::string fname)
{
  std::ifstream ifs{fname};
  dpj::zifstream zifs{zfname};
  
  std::string tok1, tok2;
  
  bool all_true = true;
  while (ifs >> tok1)
  {
    zifs >> tok2;
    all_true = all_true && tok1 == tok2;
  }
  zifs >> tok2;
  
  all_true = zifs.rdstate() == ifs.rdstate();
  
  return all_true;
}

bool test_aifstream_read_tokens(std::string zfname, std::string fname)
{
  dpj::aifstream ifs{fname};
  dpj::aifstream zifs{zfname};
  
  std::string tok1, tok2;
  
  bool all_true = true;
  while (ifs >> tok1)
  {
    zifs >> tok2;
    all_true = all_true && tok1 == tok2;
  }
  zifs >> tok2;
  
  all_true = zifs.rdstate() == ifs.rdstate();
  
  return all_true;
}


bool test_decompress_objects(std::string ref, unsigned n)
{
  bool all_true = true;
  dpj::zifstream zifs{ref};
  test_data test_obj;
  for (auto const& o : random_objects(n))
  {
    test_obj.a = 0;
    test_obj.b = "";
    test_obj.c = {0,0,0,0,0,0};
    zifs >> test_obj;
    all_true = all_true && (o == test_obj);
  }
  return all_true;
}
/// tests deflating a file.
//
bool test_compress_gzip_file(std::string ref, std::string target)
{
  {
    dpj::zofstream zof{target+".gz"};
    if (!zof.good())
      throw std::runtime_error{"couldn't open file: "+target+".gz for writing."};
    zof.rdbuf()->write_gzip_header(target, 3, false);
    std::ifstream ifs{target};
    if (!ifs.good())
      throw std::runtime_error{"couldn't open file: "+target+" for reading."};
    std::stringstream ss;
    zof << ifs.rdbuf();
  }
  return compare_files(target+".gz", ref);
}
void write_objects_and_compress(std::string name,
                                std::vector<test_data> const& objs, bool gzip)
{
  {
    std::ofstream ofs{name};
    if (!ofs.good())
      throw std::runtime_error("write_objects_and_compress: file not good: "+name);
    
    for (auto const& o : objs) ofs << o;
  }
  
  // TODO: zip the files with a third party lib to test against.
  //
  //   - this assumes that dpj::zofstream works correctly with zofs << ifs.rdbuf().
  //
  if (gzip)
  {
    dpj::zofstream zofs{name+".gz"};
    zofs.rdbuf()->write_gzip_header(name, 3, false);
    std::ifstream ifs{name};
    zofs << ifs.rdbuf();
  }
  else
  {
    dpj::zofstream zofs{name+".zlib", dpj::zofstream::action::comp_zlib};
    std::ifstream ifs{name};
    zofs << ifs.rdbuf();
  }
};

bool test_compress_objects()
{
  bool all_true = true;
  auto objs = random_objects(10);
  std::string name = "bin_10";
  {
    write_objects_and_compress(name, objs, false /* gzip */);
    dpj::zofstream zofs{name+"_test", dpj::zofstream::action::comp_zlib};
    for (auto const& o : objs) zofs << o;
  }
  all_true = all_true && compare_files(name+"_test", name+".zlib");
  {
    write_objects_and_compress(name, objs, true /* gzip */);
    dpj::zofstream zofs{name+"_test"};
    zofs.rdbuf()->write_gzip_header(name, 3, false);
    for (auto const& o : objs) zofs << o;
  }
  all_true = all_true && compare_files(name+"_test", name+".gz");
  
  name = "bin_100000";
  objs = random_objects(100000);
  {
    write_objects_and_compress(name, objs, false /* gzip */);
    dpj::zofstream zofs{name+"_test", dpj::zofstream::action::comp_zlib};
    for (auto const& o : objs) zofs << o;
  }
  all_true = all_true && compare_files(name+"_test", name+".zlib");
  {
    write_objects_and_compress(name, objs, true /* gzip */);
    dpj::zofstream zofs{name+"_test"};
    zofs.rdbuf()->write_gzip_header(name, 3, false);
    for (auto const& o : objs) zofs << o;
  }
  all_true = all_true && compare_files(name+"_test", name+".gz");
  
  return all_true;
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


