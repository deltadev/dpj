#ifndef _DPJ_ALGOS_
#define _DPJ_ALGOS_

template<std::size_t const k, typename InputIt, typename OutputIt>
void min_window_alloc(InputIt first1, InputIt const last1, OutputIt first2)
{
  typedef typename std::iterator_traits<InputIt>::value_type value_type;
  std::array<value_type, k> buf{{instrumented<int>{0}}};
  for (std::size_t i = k; first1 != last1; ++first1, ++first2, ++i)
  {
    buf[i % k] = *first1;
    std::size_t r = 1;
    while (*first1 < buf[(i - r) % k])
    {
      buf[(i - r) % k] = *first1;
      ++r;
    }
    *first2 = buf[(i + 1) % k];
  }
}

template<typename InputIt, typename OutputIt, typename BufferIt>
void min_window(std::size_t const k, InputIt first1, InputIt const last1, OutputIt first2,
                BufferIt buf)
{
  for (std::size_t i = k; first1 != last1; ++first1, ++first2, ++i)
  {
    *(buf + (i % k)) = *first1;
    std::size_t r = 1;
    while (*first1 < *(buf + (i - r) % k))
    {
      *(buf + (i - r) % k) = *first1;
      ++r;
    }
    *first2 = *(buf + (i + 1) % k);
  }
}

template<typename InputIt, typename OutputIt>
void min_window_slow(std::size_t k, InputIt first1, InputIt const last1, OutputIt first2)
{
  typedef typename std::iterator_traits<InputIt>::value_type value_type;
  first2 = first2 + (k - 1);
  for (auto seg_end = first1 + k; (seg_end - 1) != last1; ++first1, ++first2, ++seg_end)
  {
    auto min = std::numeric_limits<value_type>::max();
    
    for (auto it = first1; it != seg_end; ++it)
    {
      if (*it < min)
        min = *it;
    }
    *first2 = min;
  }
}




#endif /* _DPJ_ALGOS_ */
