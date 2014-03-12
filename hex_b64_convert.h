#ifndef DPJ_HEX_B64_CONVERT_H_
#define DPJ_HEX_B64_CONVERT_H_

#include <vector>
#include <string>

std::vector<uint8_t> hex_to_bytes(std::string const& hex);
std::vector<uint8_t> b64_to_bytes(std::string const& b64);
std::string bytes_to_hex(std::vector<uint8_t> const& bytes);


std::string bytes_to_hex(std::vector<uint8_t>::const_iterator b,
                         std::vector<uint8_t>::const_iterator e);
std::string bytes_to_b64(std::vector<uint8_t> const& bytes);


#endif /* DPJ_HEX_B64_CONVERT_H_ */
