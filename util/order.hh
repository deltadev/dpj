#ifndef DPJ_ORDER_H_
#define DPJ_ORDER_H_

#include <vector>
#include <algorithm>

namespace dpj 
{
    
  // order - obtain the order of a container.
  //
  //     If the elements in the container are 11, 14, 12, 15, 13, then the order
  //     is the sequence: 0 2 4 1 3 and the rank is 0 3 1 4 2.
  //
  //     The order gives the indices for a sorted container.
  //
  //     The rank gives the ranks of the elemnents in the container.
  //
  //     The rank is obtained as the order of the order.
  //

  template<typename Iter1, typename Iter2>
  void order(Iter1 first1, Iter1 last1, Iter2 first2) {

    typedef typename Iter2::value_type index_t;
    typedef std::pair<Iter1, index_t> pair_t;
    
    std::vector<pair_t> pairs;
    pairs.reserve(std::distance(first1, last1));

    for (index_t index = 0; first1 != last1; ++first1, ++index)
      pairs.emplace_back(first1, index);

    std::sort(pairs.begin(), pairs.end(), [](pair_t l, pair_t r) {
        return *l.first < *r.first;
      });
    
    std::transform(pairs.begin(), pairs.end(), first2, [](pair_t p) {
        return p.second;
      });
  }


  template<typename Iter1, typename Iter2>
  void stable_order(Iter1 first1, Iter1 last1, Iter2 first2) {

    typedef typename Iter2::value_type index_t;
    typedef std::pair<Iter1, index_t> pair_t;
    
    std::vector<pair_t> pairs;
    pairs.reserve(std::distance(first1, last1));

    for (index_t index = 0; first1 != last1; ++first1, ++index)
      pairs.emplace_back(first1, index);

    std::stable_sort(pairs.begin(), pairs.end(), [](pair_t l, pair_t r) {
        return *l.first < *r.first;
      });
    
    std::transform(pairs.begin(), pairs.end(), first2, [](pair_t p) {
        return p.second;
      });
  }
}

#endif /* DPJ_ORDER_H_ */
