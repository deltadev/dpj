#include "zstreambuf_test.h"

int main(int argc, const char * argv[])
{
  // file `test_text' is required and then the following:
  //
  // cp test_text zof_target && gzip -k test_text && gzipc -c zof_target > zof_ref.gz
  //
  
  try {
    std::cout << "test_decompress_gzib_file: ";
    std::cout << std::boolalpha << test_decompress_gzip_file("test_text.gz", "test_text") << '\n';
    
    std::cout << "test_decompress_read_tokens: ";
    std::cout << std::boolalpha << test_decompress_read_tokens("test_text.gz", "test_text") << '\n';
    
    std::cout << "test_aifstream_read_tokens: ";
    std::cout << std::boolalpha << test_aifstream_read_tokens("test_text.gz", "test_text") << '\n';
    
    std::cout << "test_compress_gzip_file: ";
    std::cout << std::boolalpha << test_compress_gzip_file("zof_ref.gz", "zof_target") << '\n';
    
    std::cout << "test_compress_objects: ";
    std::cout << std::boolalpha << test_compress_objects() << '\n';
    
    /* uses bin_100000.gz from earlier test */
    std::cout << "test_decompress_objects: ";
    std::cout << std::boolalpha << test_decompress_objects("bin_100000.gz", 100000) << '\n';


  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << '\n';
  }
  return 0;
}


