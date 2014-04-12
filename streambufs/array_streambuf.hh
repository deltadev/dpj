#ifndef DPJ_ARRAY_STREAMBUF_HH_
#define DPJ_ARRAY_STREAMBUF_HH_






#include <streambuf>

namespace dpj 
{
  // stream interface for extracting objects from a stream.
  //
  //   The main feature is a a method to mark the current position
  //   in the stream before an extraction operation. After clearing
  //   stream state and filling, extraction can then continue without
  //   loss of data.
  //
  //
  class array_streambuf : public std::streambuf
  {
    std::array<char, 4096> buf;
    char* c;
  public:
    array_streambuf()
    {
      clear();
    }
    
    void clear()
    {
      c = begin(buf);
      setg(begin(buf), begin(buf), begin(buf));
    }

    // underflow()
    //
    // This method will be called before and extraction fails.
    //
    // We need to:
    //
    // 1) memmove the unextracted data to the front of the buffer
    // 2) set the get ptrs to memmoved data
    // 3) return eof so that the caller knows extraction failed
    //
    int_type underflow()
    {
      if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());
     
      long remain = egptr() - c;
      std::memmove(begin(buf), c, remain);
      setg(begin(buf), begin(buf), begin(buf) + remain);

      return traits_type::eof();
    }
    
    void mark_unget() { c = gptr(); }
    
    long write(char const* first, char const* last)
    {
      auto n = std::min<long>(last - first, avail());
      std::copy(first, first + n, egptr());
      update_egptr(n);
      return n;
    }
    
    char* pubegptr() { return egptr(); }
    long avail() { return buf.size() - in_avail(); }
    void update_egptr(long n) { setg(begin(buf), gptr(), egptr() + n); }
  };
}



#endif /* DPJ_ARRAY_STREAMBUF_HH_ */
