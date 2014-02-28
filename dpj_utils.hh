#ifndef _DPJ_UTILS_HH_
#define _DPJ_UTILS_HH_

#include "order.hh"
#include "indexer.hh"
#include "stats.hh"
#include "histogram.hh"

#include <sstream>
#include <iomanip>

namespace dpj
{
  namespace str {

    std::string strip(std::string s)
    {
      auto fnb = s.find_first_not_of(" \r\n\t");
      auto lnb = s.find_last_not_of(" \r\n\t");
      return s.substr(fnb, lnb - fnb + 1);
    }

    template <typename T>
    T cast(const std::string& s)
    {
      std::stringstream ss{s};
      T thing;
      if ((ss >> thing).fail() || !(ss >> std::ws).eof())
      {
        throw std::bad_cast();
      }

      return thing;
    }
  }
}

#endif /* _DPJ_UTILS_HH_ */
