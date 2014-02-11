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
