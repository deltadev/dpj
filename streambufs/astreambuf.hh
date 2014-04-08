#ifndef DPJ_ASTREAMBUF_HH_
#define DPJ_ASTREAMBUF_HH_

#include <streambuf>

namespace dpj 
{
  // stream interface for extracting objects from a stream.
  //
  //   The main feature is a a method to mark the current position
  //   in the stream before an extraction operation. After clearing
  //   stream state and filling extraction can then continue without
  //   loss of data.
  //
  //
  class astreambuf : public std::streambuf
  {
  protected:
    char* b_, * e_, * c_;
  public:

    // Constructor to init with data.
    //
    astreambuf(char* b, char* e) : b_{b}, e_{e}, c_{e}
    {
      setg(b_, b_, e_);
    }
    
    void clear() { setg(b_, b_, b_); }

    
    int_type underflow()
    {
      if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());
     
      // Sets the get area to the unextracted data. This could
      // be extraced after clearing rdstate for instance.
      //
      std::memmove(b_, c_, e_ - c_);
      setg(b_, b_, b_ + (e_ - c_));

      return traits_type::eof();
    }
    void mark_unget()
    {
      c_ = gptr();
    }
    
    long fill(char const* first, char const* last)
    {
      // pre-condition:
      //
      // egptr() is our insertion point for new data.
      //
      auto n = std::min(std::distance(first, last), (e_ - b_) - in_avail());
      std::copy(first, first + n, egptr());
      setg(b_, b_, egptr() + n);
      return n;
    }
  };
}


#endif /* DPJ_ASTREAMBUF_HH_ */
