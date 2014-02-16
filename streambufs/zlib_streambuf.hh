#ifndef _ZLIB_STREAMBUF_HH_
#define _ZLIB_STREAMBUF_HH_

#include <zlib.h>

#include <streambuf>
#include <vector>


namespace dpj {
  
  ///
  // At the moment this is only set up to inflate.
  //
  class zlib_streambuf : public std::streambuf
  {
    z_stream zs;
    int z_state = Z_OK;

    std::streambuf* in_sbuf;
    size_t const BUF_SIZE;
    std::vector<char> get_buf;
    std::vector<char> in_buf;
    
  public:

    zlib_streambuf(std::streambuf* in_sbuf, std::size_t buf_size = 1 << 15 /* 32K */)

    : in_sbuf(in_sbuf)
    , BUF_SIZE(buf_size)
    , get_buf(BUF_SIZE)
    , in_buf(BUF_SIZE)

    {
      setg(get_buf.data(), get_buf.data(), get_buf.data());

      // This calls the zlib inflateInit2. It relies on the implementation detail that,
      //
      // "The current implementation of inflateInit2() does not process any header information
      //   -- that is deferred until inflate() is called."
      //
      // since at this stage there is no compressed data in the input buffer.
      //
      inflate_init();
    }
    

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

  private:
    bool done_init = false;
    
    std::streamsize fill()
    {

      // Assumes the get buffer is exhausted.
      //
      // underflow() only calls this method when gptr() == egptr().
      //
      zs.next_out = (uint8_t*)get_buf.data();
      zs.avail_out = static_cast<unsigned>(get_buf.size());
      

      // inflate()
      //
      //   - Runs inflate until we get some output.
      //   - Returns zero bytes if we get Z_STREAM_END.
      //
      //
      // This appears to be different to the method described here:
      //
      // http://zlib.net/zlib_how.html
      //
      // where inflate is called until the output buffer, (get_buf in our case), is
      // full.
      //
      // The strategy here is to call inflate only until we get some output.
      //
      while(get_buf.size() - zs.avail_out == 0)
      {
        if (z_state == Z_STREAM_END)
        {
          inflateEnd(&zs);
          return 0;
        }

        if (zs.avail_in == 0)
        {
          zs.avail_in = static_cast<unsigned>(in_sbuf->sgetn(in_buf.data(), in_buf.size()));
          zs.next_in = (uint8_t*)in_buf.data();
        }
        

        while (zs.avail_in > 0 && zs.avail_out > 0 && z_state == Z_OK)
          z_state = inflate(&zs, Z_NO_FLUSH);
        
        
        if (z_state != Z_OK && z_state != Z_STREAM_END)
        {
          throw std::runtime_error("zlib_streambuf::fill inflate: "+std::string(zError(z_state)));
        }
        
      }
      return get_buf.size() - zs.avail_out; // Number of bytes written to get_buf.
    }
    
    void inflate_init()
    {
      if (!done_init)
      {
        zs.zalloc = Z_NULL;
        zs.zfree = Z_NULL;
        zs.opaque = Z_NULL;

        zs.next_in = (uint8_t*)in_buf.data();
        zs.avail_in = 0;
        
        // 15 + 32
        //
        // 15 - for the history buffer size: 2^15 == 32K, this needs to correspond to
        //      the size of the histroy buffer used in the deflate compression. Setting it too
        //      small will result in the back pointers pointing too far back. A likely
        //      Z_DATA_ERROR will result with the error message, "invalid distance too far back".
        //
        // 32 - to enable zlib and gzip decoding with automatic header detection.
        //
        int r = inflateInit2(&zs, 15 + 32);
        
        
        if (r != Z_OK)
          throw std::runtime_error("zlib_streambuf: " + std::string(zError(r)));
        
        done_init = true;
      }
    }
  };
}

#endif /* _ZLIB_STREAMBUF_HH_ */
