#ifndef _ZLIB_HH_
#define _ZLIB_HH_

#include <zlib.h>

namespace dpj { namespace zlib
  {
    enum class error
    {
      Z_OK = 0,
      Z_STREAM_END = 1,
      Z_NEED_DICT = 2,
      Z_ERRNO = -1,
      Z_STREAM_ERROR = -2,
      Z_DATA_ERROR = -3,
      Z_MEM_ERROR = -4,
      Z_BUF_ERROR = -5,
      Z_VERSION_ERROR = -6
    };
    
    std::string error_str(error const& e)
    {
      switch (e) {
        case Z_OK: return "Z_OK";
        case Z_STREAM_END: return "Z_STREAM_END";
        case Z_NEED_DICT: return "Z_NEED_DICT";
        case Z_ERRNO: return "Z_ERRNO";
        case Z_STREAM_ERROR: return "Z_STREAM_ERROR";
        case Z_DATA_ERROR: return "Z_DATA_ERROR";
        case Z_MEM_ERROR: return "Z_MEM_ERROR";
        case Z_BUF_ERROR: return "Z_BUF_ERROR";
        case Z_VERSION_ERRO: return "Z_VERSION_ERRO";
        default:
          throw std::runtime_error
      }
    }
    
  }
#endif /* _ZLIB_HH_ */
