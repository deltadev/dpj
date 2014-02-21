#include "hex_b64_convert.h"

#include <iostream>

using std::cout;
using std::cerr;

typedef uint8_t u8;

std::vector<uint8_t> hex_to_bytes(std::string const& hex)
{
  // Deals with the case hex.size() == 1 (mod 2).
  //
  std::string hhex;
  if (hex.size() % 2)
    hhex.append("0");
  hhex.append(hex);


  std::vector<uint8_t> bs; bs.reserve(hhex.size() / 2);
  u8 b = 0, shift = 4;
  for (auto c : hhex) {
    if      ( '0' <= c && c <= '9' )  b |=        (c & 0x0f) << shift;
    else if ( 'A' <= c && c <= 'F' )  b |= (0x9 + (c & 0x0f)) << shift;
    else if ( 'a' <= c && c <= 'f' )  b |= (0x9 + (c & 0x0f)) << shift;
    else {
      cerr << "ERROR: Unknown input char :" << c << ".\nExiting...\n";
      exit(1);
    }
    
    if ((shift = (shift + 4) % 8)) { // toggle shift
      bs.push_back(b);
      b = 0;
    }
  }
  return bs; // move
}

std::vector<uint8_t> b64_to_bytes(std::string const& b64)
{
  // Have not dealt with the case hex.size() == 1 (mod 2).
  //
  if (b64.size() % 4) {
    cerr << "ERROR: Require 4n b64 chars as input!\nExiting...\n";
    exit(1);
  }

  std::vector<uint8_t> bs; bs.reserve((b64.size() * 6 + 8 - 1) / 8);

  typedef std::string::const_iterator It;
  auto eat_b64 = [&bs](It b, It e) {
    // Eats 4 or fewer tokens.
    
    auto b64_tok_to_uint = [](char c) -> u8 {
      u8 u = 0;
      if      ( 'A' <= c && c <= 'Z' )  u = c - 'A';
      else if ( 'a' <= c && c <= 'z' )  u = c + 26 - 'a';
      else if ( '0' <= c && c <= '9' )  u = c + 52 - '0';
      else if ( c == '+' )              u = 62;
      else if ( c == '/' )              u = 63;
      return u;
    };
    
    uint32_t bbuf = 0;
    for (size_t i = 0; i < 4; ++i)
        bbuf |= b64_tok_to_uint(*b++) << (3 - i) * 6;
    
    bs.push_back((bbuf & 0x00ff0000) >> 16);
    bs.push_back((bbuf & 0x0000ff00) >> 8);
    bs.push_back((bbuf & 0x000000ff));
  };
  
  for (auto it = b64.begin(); it != b64.end(); it += 4)
    eat_b64(it, it + 4);
  
  // Remove bytes for any '=' padding.
  //
  size_t n = std::count(b64.begin(), b64.end(), '=');
  bs.resize(bs.size() - n);
  return bs;
}
std::string bytes_to_hex(std::vector<uint8_t>::const_iterator b, std::vector<uint8_t>::const_iterator e) {
  std::string hex; hex.reserve((e - b) * 2);
  auto uint_to_hex = [](u8 u) -> char { return u < 10 ? '0' + u : 'a' - 10 + u; };
  std::for_each(b, e, [&](u8 u) {
    hex.push_back(uint_to_hex((u & 0xf0) >> 4));
    hex.push_back(uint_to_hex(u & 0x0f));
  });
  return hex;
}

std::string bytes_to_hex(std::vector<uint8_t> const& bytes) { return bytes_to_hex(bytes.begin(), bytes.end()); }

std::string bytes_to_b64(std::vector<uint8_t> const& bytes)
{
  std::string b64; b64.reserve((bytes.size() * 8 + 6 - 1) / 6);

  typedef std::vector<uint8_t>::const_iterator It;
  auto eat_three_bytes = [&b64](It b) {
    
    auto uint_to_b64 = [](u8 u) -> char {
      char c;
      if (u < 26)       c = 65 + u;
      else if (u < 52)  c = u - 26 + 97;
      else if (u < 62)  c = u - 52 + 48;
      else if (u == 62) c = '+';
      else              c = '/';
      return c;
    };
    
    u8 toks[4] {0};
    toks[0]   = (*b   & 0xfc) >> 2; // bbbbbb.. |-> ..bbbbbb
    b64.push_back(uint_to_b64(toks[0]));
    
    toks[1]   = (*b++ & 0x03) << 4; // ......bb |-> ..bb....
    toks[1]  |= (*b   & 0xf0) >> 4; // bbbb.... |-> ..bb.... + ....bbbb == ..bbbbbb
    b64.push_back(uint_to_b64(toks[1]));
    
    toks[2]   = (*b++ & 0x0f) << 2; // ....bbbb |-> ..bbbb..
    toks[2]  |= (*b   & 0xc0) >> 6; // bb...... |-> ..bbbb.. + ......bb == ..bbbbbb
    b64.push_back(uint_to_b64(toks[2]));
    
    toks[3]   = (*b++ & 0x3f) >> 0; // ..bbbbbb |-> ..bbbbbb
    b64.push_back(uint_to_b64(toks[3]));
        
  };
  
  for (auto b = bytes.begin(); b + 2 < bytes.end(); b += 3)
    eat_three_bytes(b);
  
  // Deals with overlap if present. Messy messy.
  //
  if (size_t bytes_to_pad = (bytes.size() % 3))
  {
    std::vector<uint8_t> padding(bytes.end() - bytes_to_pad, bytes.end());
    padding.resize(3, 0);
    eat_three_bytes(padding.begin());
    auto last = b64.rbegin();
    *last++ = '=';
    if (bytes_to_pad == 1) {
      *last = '=';
    }
  }
  return b64;
}





























