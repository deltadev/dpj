#ifndef DPJ_ALGOS_
#define DPJ_ALGOS_

#include <iterator>


template<typename Int, typename It>
void min_window_simple(Int const w, It first1, It const last1, It first2)
{
  typedef typename std::iterator_traits<It>::value_type value_type;
  for (auto seg_end = first1 + w; (seg_end - 1) != last1; ++first1, ++first2, ++seg_end)
  {
    // FIXME: If value_type is std::pair then how do we get what we need here?
    //
    auto min = value_type{std::numeric_limits<int>::max()};
    
    for (auto it = first1; it != seg_end; ++it)
    {
      if (*it < min)
        min = *it;
    }
    *first2 = min;
  }
}

template<typename Queue, typename Int, typename It>
void min_window_queue(Queue& q, Int w, It first1, It const last1, It first2)
{
  typedef typename std::iterator_traits<It>::value_type value_type;
  
  Int i = 0;
  
  // We add the first w elements without doing any output.
  //
  for (; i < w; ++i, ++first1)
  {
    while (!q.empty() && *first1 < q.back().first)
      q.pop_back();
    q.push_back({*first1, i});
  }
  for (; first1 != last1; ++i, ++first1)
  {
    if (!q.empty())
    {
      auto front = q.front();
      
      *first2 = front.first;
      ++first2;
        
      if (front.second < i - w + 1)
        q.pop_front();
    
      // First check to see if this is smaller than the whole queue. If
      // it is we can just reset the queue with this value at the front.
      //
      if (*first1 < front.first)
      {
        q.clear();
      }
      else
      {
        // We don't need to check the queue is empty since we are guarded
        // by a smaller element, given the check above.
        //
        while (*first1 < q.back().first)
          q.pop_back();
      }
    }

    // Now we can push our new value.
    //
    q.push_back({*first1, value_type{i}});

  }
  *first2 = q.front().first;
}




#endif /* DPJ_ALGOS_ */
