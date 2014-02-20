#ifndef _ZSTREAMBUF_HH_
#define _ZSTREAMBUF_HH_

#include <zlib.h>

#include <streambuf>
#include <array>


namespace dpj {
  

  /// Large buffer reading and writing of zlib and gzip streams.
  //
  //   This is a simplified interface, e.g.
  //
  //     - no seeking
  //
  template <std::size_t BUF_SIZE = 256 * 1024 /* 256K */>
  class zstreambuf_t : public std::streambuf
  {

  public:
    
    zstreambuf_t(std::streambuf* io_sbuf, std::ios_base::openmode m = std::ios_base::in)
    : mode(m), io_sbuf(io_sbuf)
    {
      if (mode == std::ios_base::in)
      {
        setg(ibegin, ibegin, ibegin);
        
        // This calls the zlib inflateInit2. It relies on the implementation detail that,
        //
        // "The current implementation of inflateInit2() does not process any header information
        //   -- that is deferred until inflate() is called."
        //
        // since at this stage there is no compressed data in the input buffer.
        //
        inflate_init();
      }
      else if (mode == std::ios_base::out)
      {

        setp(ibegin, ibegin + BUF_SIZE);
        deflate_init();
      }
      else
      {
        throw std::runtime_error("zstreambuf must be used exclusively in in or out mode");
      }
    }
    
    // Later, allow user to provide data to create a more complete gzip header.
    //
    // For now; this will give a basic gzip header with no filename etc.
    //
    void write_gzip(std::string const& fname, bool tag = true)
    {
      gzip_write_mode = true;
      deflate_init();
      
      gz_header head;
      head.text = false;         // Agnostic.
      head.time = clock();
      head.os = 3;               // Unix. What else?
      head.extra = Z_NULL;
      
      head.name = (uint8_t*)fname.c_str();
      
      if (tag)
        head.comment = (uint8_t*)"written by dpj::zstreambuf";

      head.hcrc = 1; // no CRC at the mo.
      
      int r = deflateSetHeader(&zs, &head);
      if (!r == Z_OK)
      {
        throw std::runtime_error("zstreambuf: couldn't write gzip header");
      }

      setp(ibegin, ibegin);
      deflate_init();
    }
    
  protected:
    
    int_type underflow()
    {
      if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());
      
      std::streamsize n = fill();

      setg(ibegin, ibegin, ibegin + n);
      
      if (n != 0)
        return traits_type::to_int_type(*gptr());
      else
        return traits_type::eof();
    }
    
    int_type overflow(int_type ch)
    {
      if (!traits_type::eq_int_type(ch, traits_type::eof()))
      {
        std::streamsize n = flush();
        if (n < ebuf.size())
        {
          return traits_type::eof();
        }
        else
        {
          setp(ibegin, ibegin + n);
          pbump(ch);
        }
        return ch;
      }
      else
        return traits_type::eof();
    }
    
    int sync()
    {
      int r = -1;
      if (mode == std::ios_base::in)
      {
        // TODO:
        //r = inflateReset(&zs);
      }
      else if (mode == std::ios_base::out)
      {
        // TODO
        //r = deflateReset(&zs);
      }
      return r;
    }
    
  private:
    z_stream zs;
    
    int z_state = Z_OK;

    bool gzip_write_mode = false;
    bool done_init = false;
    
    std::ios_base::openmode mode;
    std::streambuf* io_sbuf;
    
    std::array<unsigned char, BUF_SIZE> ebuf;
    std::array<unsigned char, BUF_SIZE> ibuf;
    char* ibegin = reinterpret_cast<char*>(ibuf.begin());
    char* iend = reinterpret_cast<char*>(ibuf.end());
    char* ebegin = reinterpret_cast<char*>(ebuf.begin());
    char* eend = reinterpret_cast<char*>(ebuf.end());
    std::streamsize fill()
    {
      
      // Assumes the get buffer is exhausted.
      //
      // underflow() only calls this method when gptr() == egptr().
      //
      zs.next_out = ibuf.begin();
      zs.avail_out = static_cast<unsigned>(ibuf.size());
      
      
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
      while(ibuf.size() - zs.avail_out == 0)
      {
        if (z_state == Z_STREAM_END)
        {
          inflateEnd(&zs);
          return 0;
        }
        
        if (zs.avail_in == 0)
        {
          zs.avail_in = static_cast<unsigned>(io_sbuf->sgetn(ebegin, BUF_SIZE));
          zs.next_in = ebuf.begin();
        }
        
        
        while (zs.avail_in > 0 && zs.avail_out > 0 && z_state == Z_OK)
          z_state = inflate(&zs, Z_NO_FLUSH);
        
        
        if (z_state != Z_OK && z_state != Z_STREAM_END)
        {
          throw std::runtime_error("zlib_streambuf::fill inflate: "+std::string(zError(z_state)));
        }
        
      }
      return ibuf.size() - zs.avail_out; // Number of bytes written to get_buf.
    }
    
    std::streamsize flush()
    {
      // Compresses what is in the ibuf and resets the put ptrs.
     
      std::streamsize n = pbase() - pptr();

      zs.avail_in = static_cast<unsigned>(n);
      zs.next_in = ibuf.begin();
      
      setp(ibegin, ibegin);
      
      int r = Z_OK;
      while (zs.avail_in > 0 && r == Z_OK)
      {
        r = deflate(&zs, Z_NO_FLUSH);
      }

      if (zs.avail_out == 0)
      {
        io_sbuf->sputn(ebegin, BUF_SIZE);
        zs.next_out = ebuf.begin();
        zs.avail_out = BUF_SIZE;
      }
      
      return n;
    }

    void common_init()
    {
      zs.zalloc = Z_NULL;
      zs.zfree = Z_NULL;
      zs.opaque = Z_NULL;
    }
    void inflate_init()
    {
      if (!done_init)
      {
        common_init();
        
        zs.next_in = static_cast<uint8_t*>(ebuf.begin());
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
    void deflate_init()
    {
      if (!done_init)
      {
        common_init();
        
        zs.avail_in = 0;
        zs.next_in = static_cast<uint8_t*>(ibuf.begin());
        
        int window_bits = gzip_write_mode ? 15 + 16 : 15;
        
        int r = deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, window_bits, 8,
                             
                             // This is the interesting parameter here.
                             //
                             // TODO: look into Z_RLE and Z_FILTERED.
                             //
                             Z_DEFAULT_STRATEGY);
        
        
        if (r != Z_OK)
          throw std::runtime_error("zstreambuf: failed in deflateInit2");

      }
    }
  };
  
  typedef zstreambuf_t<> zstreambuf;
  
}

#endif /* _ZLIB_STREAMBUF_HH_ */
