#ifndef _ZLIB_STREAMBUF_HH_
#define _ZLIB_STREAMBUF_HH_

#include <zlib.h>

#include <streambuf>
#include <vector>


namespace dpj {
  
  class zlib_streambuf : public std::streambuf
  {
    z_stream zs;

    std::streambuf* in_sbuf;
    size_t const BUF_SIZE;
    std::vector<char> get_buf;
    std::vector<char> in_buf;
    
  public:

    zlib_streambuf(std::streambuf* in_sbuf, std::size_t buf_size = 1 << 2)
      :
        in_sbuf(in_sbuf),
        BUF_SIZE(buf_size),
        get_buf(BUF_SIZE),
        in_buf(BUF_SIZE) { setg(get_buf.data(), get_buf.data(), get_buf.data()); }
    

  protected:
    
    int_type underflow()
    {
      if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());
      
      std::streamsize n = fill();
      setg(get_buf.data(), get_buf.data(), get_buf.data() + n);
      
      if (n != 0)
        return traits_type::to_int_type(*gptr());
      else
        return traits_type::eof();
    }

    std::streamsize xsgetn(char* s, std::streamsize n) {
      return 5;
    }
  private:
    bool done_init = false;
    
    std::streamsize fill()
    {
      // Assumes the get buffer is exhausted.
      zs.next_out = (uint8_t*)get_buf.data();
      zs.avail_out = static_cast<unsigned>(get_buf.size());
      
      int r = Z_OK;
      while (get_buf.size() - zs.avail_out == 0)
      {
        // inflate_init() must only be called after an initial fill.
        if (zs.avail_in == 0)
        {
          zs.avail_in = static_cast<unsigned>(in_sbuf->sgetn(in_buf.data(), in_buf.size()));
          zs.next_in = (uint8_t*)in_buf.data();
        }

        inflate_init(); // Should only be called the first time.
        
        while (zs.avail_in > 0 && zs.avail_out > 0 && r == Z_OK)
          r = inflate(&zs, Z_NO_FLUSH);
        
        if (r != Z_OK && r != Z_STREAM_END) {
          std::string msg = zError(r);
          throw std::runtime_error("zlib_streambuf::fill inflate: "+msg);
        }
      }
      return get_buf.size() - zs.avail_out; // Number of bytes written to get_buf.
    }
    
    void inflate_init()
    {
      if (!done_init)
      {
        int r = inflateInit2(&zs, 32);
        if (r != Z_OK)
          throw std::runtime_error("zlib_streambuf: " + std::string(zError(r)));
        done_init = true;
      }
    }
  };
}

#endif /* _ZLIB_STREAMBUF_HH_ */
