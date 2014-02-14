#include <iostream>
#include <streambuf>
#include <vector>
#include <fstream>

class streambuf_switcher : public std::streambuf
{
  std::vector<char> b0;
  std::vector<char> b1;
  
  bool in_b0 = true;
public:
  
  streambuf_switcher(std::string a, std::string b);
protected:
  int_type underflow();
  pos_type seekoff(off_type, std::ios_base::seek_dir, std::ios_base::open_mode);
  pos_type seekpos(pos_type, std::ios_base::open_mode);
};



class strange_stream : public std::istream
{
  streambuf_switcher sbs;
public:
  strange_stream(std::string a, std::string b) : sbs(a, b), std::istream(&sbs) {  }
};


int main(int argc, char *argv[])
{
  
  strange_stream ss{"  fiz bang pop ", "      foo bar baz   "};
  
  std::string s;
  while (ss >> s) {

    std::cout << s << ' ';


    if (random() % 10 == 1) {
      ss.seekg(random());
      std::cout << '\n' << (unsigned)ss.tellg() << '\n';
    }
  }
  
  return 0;
}


streambuf_switcher::streambuf_switcher(std::string a, std::string b)
    : b0(a.begin(), a.end()), b1(b.begin(), b.end())
{ setg(b0.data(), b0.data(), b0.data() + b0.size()); }


streambuf_switcher::int_type streambuf_switcher::underflow() {
  
  if (gptr() != egptr())
    return traits_type::int_type(*gptr());
  
  if (in_b0)
    setg(b1.data(), b1.data(), b1.data() + b1.size());
  else
    setg(b0.data(), b0.data(), b0.data() + b0.size());
  
  in_b0 = !in_b0;
  
  return traits_type::int_type(*eback());
}


streambuf_switcher::pos_type
streambuf_switcher::seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode)
{
  
  auto sz0 = b0.size();
  auto sz1 = b1.size();
  
  if (way == std::ios_base::cur) {
    
    
    pos_type here = gptr() - eback();
    if (!in_b0) here += sz0;
    
    
    off_type new_pos = (here + off) % (sz0 + sz1);
    
    // If the new pos is in a different buffer we need to call toggle.
    if (new_pos < sz0) {
      in_b0 = true;
      setg(b0.data(), b0.data() + new_pos, b0.data() + sz0);
    } else {
      in_b0 = false;
      setg(b1.data(), b1.data() + (new_pos - sz0), b1.data() + sz1);
    }    
    
    return new_pos;
    
  } else {
    throw std::runtime_error("we only support seekoff from ios_bas::cur");
  }
}

streambuf_switcher::pos_type
streambuf_switcher::seekpos(pos_type pos, std::ios_base::openmode) {
  
  auto sz0 = b0.size();
  auto sz1 = b1.size();
  
  off_type new_pos = pos % (sz0 + sz1);
  if (new_pos < sz0) {
    in_b0 = true;
    setg(b0.data(), b0.data() + new_pos, b0.data() + sz0);
  } else {
    in_b0 = false;
    setg(b1.data(), b1.data() + (new_pos - sz0), b1.data() + sz1);
  }
  return new_pos;
}
