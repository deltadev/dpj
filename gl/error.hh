#ifndef DPJ_ERROR_HH_
#define DPJ_ERROR_HH_

namespace dpj {
  namespace gl {
    
    enum class error
    {
      no_error = GL_NO_ERROR,
      invalid_enum = GL_INVALID_ENUM,
      invalid_value = GL_INVALID_VALUE,
      invalid_operation = GL_INVALID_OPERATION,
      out_of_memory = GL_OUT_OF_MEMORY,
    };
    
    error get_error()
    {
      GLenum e = glGetError();
      switch(e)
      {
        case static_cast<GLenum>(error::no_error):
          return error::no_error;
        case static_cast<GLenum>(error::invalid_enum):
          return error::invalid_enum;
        case static_cast<GLenum>(error::invalid_value):
          return error::invalid_value;
        case static_cast<GLenum>(error::invalid_operation):
          return error::invalid_operation;
        case static_cast<GLenum>(error::out_of_memory):
          return error::out_of_memory;
      }
      throw std::runtime_error{"get_error: unknown error"};
    }
    struct exception : std::exception {};
    
    struct invalid_enum : exception
    { virtual char const* what() { return "invalid_enum"; } };
    struct invalid_value : exception
    { virtual char const* what() { return "invalid_value"; } };
    struct invalid_operation : exception
    { virtual char const* what() { return "invalid_operation"; } };
    struct out_of_memory : exception
    { virtual char const* what() { return "out_of_memory"; } };
    
    void check_error()
    {
      switch (get_error())
      {
        case error::invalid_enum:      throw invalid_enum{};
        case error::invalid_value:     throw invalid_value{};
        case error::invalid_operation: throw invalid_operation{};
        case error::out_of_memory:     throw out_of_memory{};
        case error::no_error:          break;
      }
    }
  }
}

#endif /* DPJ_ERROR_HH_ */
