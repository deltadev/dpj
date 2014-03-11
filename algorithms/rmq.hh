#ifndef _DPJ_RMQ_HH_
#define _DPJ_RMQ_HH_

#include <vector>

template<typename T>
class reduced_mins
{
  std::size_t n;     // number of Ts to reduce.
  std::size_t q; // interval size, floor(sqrt(n)).
  typename std::vector<T> q_mins;
public:

  template<typename It>
  reduced_mins(It first, It const last)
  {
    n = std::distance(first, last);
    q = std::sqrt(n);

    q_mins.reserve(n);
    for (auto const eit = last - q; first < eit; first += q)
    {
      auto m = *std::min_element(first, first + q);
      q_mins.push_back(m);
    }
    
    if ((first = std::min_element(first, last)) != last)
      q_mins.push_back(*first);
  }
  
  template<typename It>
  T rmq(It first, std::size_t a, std::size_t b)
  {
    //       a               b
    //       v               v
    // +---+---+---+---+---+---+---+---+---+
    //         ^           ^
    //       q * m_a     q * m_b
    //
    
    // a = m_a * q - r_a
    //
    std::size_t m_a = (a + q - 1) / q; // ceiling
    std::size_t r_a = m_a * q - a;
    
    // b = m_b * q + r_b
    //
    std::size_t m_b = b / q; // floor
    std::size_t r_b = b - m_b * q;
    
    T min = std::numeric_limits<T>::max();
    if (0 < r_a)
      min = std::min(min, *std::min_element(first + a, first + a + r_a));
    if (m_a < m_b)
      min = std::min(min, *std::min_element(begin(q_mins) + m_a, begin(q_mins) + m_b));
    if (0 < r_b)
      min = std::min(min, *std::min_element(first + (b - r_b), first + b));

    return min;
  }
};



#endif /* _DPJ_RMQ_HH_ */
