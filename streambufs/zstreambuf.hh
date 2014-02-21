#ifndef _ZSTREAMBUF_HH_
#define _ZSTREAMBUF_HH_

#include <zlib.h>

#include <streambuf>
#include <array>

#include <cassert>


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
    z_stream zs;
    
    int z_state = Z_OK;
    
    bool gzip_write_mode = false;
    bool done_init = false;
    
    std::ios_base::openmode mode;
    std::streambuf* io_sbuf;
    
    std::array<unsigned char, BUF_SIZE> z_in_buf;
    std::array<unsigned char, BUF_SIZE> z_out_buf;
    
    char* z_in_b;
    char* z_in_e;
    char* z_out_b;
    char* z_out_e;

  public:
    
    zstreambuf_t(std::streambuf* io_sbuf, std::ios_base::openmode m = std::ios_base::in)
    : mode(m), io_sbuf(io_sbuf)
    {
      // One can not static_cast<uint8_t*> to char*.
      //
      z_in_b = reinterpret_cast<char*>(z_in_buf.begin());
      z_in_e = reinterpret_cast<char*>(z_in_buf.end());
      z_out_b = reinterpret_cast<char*>(z_out_buf.begin());
      z_out_e = reinterpret_cast<char*>(z_out_buf.end());
      
      if (mode == std::ios_base::in)
      {
        setg(z_out_b, z_out_b, z_out_b);
        
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

        setp(z_in_b, z_in_e);
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

      setp(z_in_b, z_in_e);
      deflate_init();
    }
    
  protected:
    
    // underflow()
    //
    //   - this method replenishes the get area when we've run out of decompressed
    //     data. This is for decompression; hence it calls the inflate_buffer() method.
    //
    int_type underflow()
    {
      if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());
      
      std::streamsize n = inflate_buffer();


      setg(z_out_b, z_out_b, z_out_b + n);
      
      if (n != 0)
        return traits_type::to_int_type(*gptr());
      else
        return traits_type::eof();
    }
    
    /// overflow()
    //
    //   - this method deals with the situation where we have filled the input buffer
    //     and we need to deflate what is there to make room for more input.
    //
    int_type overflow(int_type ch)
    {
      if (!traits_type::eq_int_type(ch, traits_type::eof()))
      {
        if (pptr() < epptr())
        {
          return ch;
        }
        else
        {
          deflate_buffer(ch);
          setp(z_in_b, z_in_e);
          return ch;
        }
      }
      else
      {
        setp(z_in_b, z_in_b);
        return traits_type::eof();
      }
    }
    

    /// synch() - called by std::streambuf::pubsync()
    //
    int sync()
    {

      if (mode == std::ios_base::in)
      {
        // TODO: What shoud we do with synch on compressed input?
        //         - We can not necessarily decompress all remaining data at z_in_buf since there
        //           may not be space in z_out_buf for it.
        //         - Can we empty z_out_buf in that case?
        //
        //
      }
      else if (mode == std::ios_base::out)
      {
        deflate_buffer(traits_type::eof());
        z_state = deflateReset(&zs);
      }

      if (z_state != Z_OK)
        return -1;
      else
        return 0;
    }
    
  private:

    /// Pushes whatever is in our put area into the zstream.
    //
    //    - we might be called by overflow, in this case ch will be equal to either,
    //
    //    i) !traits_type::eof(), so deflate(&zs, Z_FINISH)
    //    ii) traits_type::eof(), so deflate(&zs, Z_NO_FLUSH)
    //
    //    - we might be called by sync, in this case ch == traits_type::eof().
    //
    void deflate_buffer(int_type ch)
    {
      // Everything in the put area is available.
      //
      zs.avail_in = static_cast<unsigned>(pptr() - pbase());
      zs.next_in = z_in_buf.begin();
      
      // We call deflate() until we _do_not_ fill the output buffer.
      //
      //   - if do not fill the output buffer then deflate() _will_ have
      //     consumed all input _or_ error.

      // Decides whether we are to finalise the deflate/zlib/gzip stream.
      //
      bool is_eof = traits_type::eq_int_type(ch, traits_type::eof());
      int flush = is_eof ? Z_FINISH : Z_NO_FLUSH;
      
      // pre-condition:
      //
      assert(zs.avail_out > 0);
      do // do consume input until output buffer is full.
      {
        zs.next_out = z_out_buf.begin();
        zs.avail_out = static_cast<unsigned>(z_out_buf.size());
        
        z_state = deflate(&zs, flush);
        
        io_sbuf->sputn(z_out_b, z_out_buf.size() - zs.avail_out);
      }
      while // while the output buffer is full consume more input.
        (zs.avail_out == 0);
      
      // post-condition:
      //
      assert(zs.avail_in == 0);

      // Resets the our put buffer.
      //
      setp(z_in_b, z_in_e);
    }
    
    std::streamsize inflate_buffer()
    {
      
      // Assumes the get buffer is exhausted.
      //
      // underflow() only calls this method when gptr() == egptr().
      //
      zs.next_out = z_out_buf.begin();
      zs.avail_out = static_cast<unsigned>(z_out_buf.size());
      
      
      // inflate()
      //
      //   - Runs inflate until we get some output.
      //   - Returns zero bytes if we get Z_STREAM_END.
      //
      while(z_out_buf.size() - zs.avail_out == 0)
      {
        if (z_state == Z_STREAM_END)
        {
          inflateEnd(&zs);
          return 0;
        }
        
        if (zs.avail_in == 0)
        {
          zs.avail_in = static_cast<unsigned>(io_sbuf->sgetn(z_in_b, z_in_buf.size()));
          zs.next_in = z_in_buf.begin();
        }
        
        
        while (zs.avail_in > 0 && zs.avail_out > 0 && z_state == Z_OK)
          z_state = inflate(&zs, Z_NO_FLUSH);
        
        
        if (z_state != Z_OK && z_state != Z_STREAM_END)
        {
          throw std::runtime_error("zlib_streambuf::inflate_buffer inflate: "+std::string(zError(z_state)));
        }
        
      }
      return z_out_buf.size() - zs.avail_out; // Number of bytes written to get_buf.
    }
    
    /// Initialisateion routines.
    //
    //   - setup
    //
    void common_init()
    {
      zs.zalloc = Z_NULL;
      zs.zfree = Z_NULL;
      zs.opaque = Z_NULL;
      
      zs.next_in = z_in_buf.begin();
      zs.avail_in = 0;
      
      zs.next_out = z_out_buf.begin();
      zs.avail_out = static_cast<unsigned>(z_out_buf.size());
    }
    
    void inflate_init()
    {
      if (!done_init)
      {
        common_init();
        
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
