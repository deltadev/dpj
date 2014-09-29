#ifndef DPJ_GET_HH_
#define DPJ_GET_HH_

#include "error.hh"

namespace dpj {
  namespace gl {
    
    
    GLint active_texture_unit()
    {
      GLint data = 0;
      glGetIntegerv(GL_ACTIVE_TEXTURE, &data);
      check_error();
      
      return data;
    }
    
  } // ns gl
  
} // ns dpj

#endif /* DPJ_GET_HH_ */
