#ifndef _DPJ_UTILS_H_
#define _DPJ_UTILS_H_

#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <numeric>

#include <unordered_set>

namespace dpj {
    
  //
  // http://stackoverflow.com/a/10962575/710408
  //
  
  template <typename T>
  struct iterator_extractor { typedef typename T::iterator type; };
  
  template <typename T>
  struct iterator_extractor<T const> { typedef typename T::const_iterator type; };
  
  
  template <typename T>
  class Indexer {
  public:
    class iterator {
      typedef typename iterator_extractor<T>::type inner_iterator;
      
      typedef typename std::iterator_traits<inner_iterator>::reference inner_reference;
    public:
      typedef std::pair<size_t, inner_reference> reference;
      
      iterator(inner_iterator it): _pos(0), _it(it) {}
      
      reference operator*() const { return reference(_pos, *_it); }
      
      iterator& operator++() { ++_pos; ++_it; return *this; }
      iterator operator++(int) { iterator tmp(*this); ++*this; return tmp; }
      
      bool operator==(iterator const& it) const { return _it == it._it; }
      bool operator!=(iterator const& it) const { return !(*this == it); }
      
    private:
      size_t _pos;
      inner_iterator _it;
    };
    
    Indexer(T& t): _container(t) {}
    
    iterator begin() const { return iterator(_container.begin()); }
    iterator end() const { return iterator(_container.end()); }
    
  private:
    T& _container;
  }; // class Indexer
  
  template <typename T>
  Indexer<T> index(T& t) { return Indexer<T>(t); }
  

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
  

  template <typename Iter1, typename Iter2>
  double covariance(Iter1 first1, Iter1 last1, Iter2 first2) {
    double n = std::distance(first1, last1);
    double s_x = 0;
    double s_y = 0;
    double s_xy = 0;
    while (first1 != last1) {
      s_x += *first1;
      s_y += *first2;
      s_xy += *first1 * *first2;
      ++first1;
      ++first2;
    }
    // \Sigma{xy} / n - \bar{x}\bar{y})
    return (s_xy / n - (s_x / n) * (s_y / n)) * n / (n - 1);
  }

  template <typename Iter1, typename Iter2>
  double pearsons_cc(Iter1 first1, Iter1 last1, Iter2 first2) {
    double n = std::distance(first1, last1);
    double s_x = 0;
    double s_y = 0;
    double s_xy = 0;
    double s_xx = 0;
    double s_yy = 0;
    while (first1 != last1) {
      s_x += *first1;
      s_y += *first2;
      s_xy += *first1 * *first2;
      s_xx += *first1 * *first1;
      s_yy += *first2 * *first2;
      ++first1;
      ++first2;
    }
    // \Sigma{xy} / n - \bar{x}\bar{y}) / sqrt(var(x)var(y))
    // (n * Sxx - Sx * Sy) / sqrt((nSxx - Sx * Sx) * (Syy - Sy * Sy))
    return (n * s_xy - s_x * s_y) / std::sqrt( (n * s_xx - s_x * s_x) * (n * s_yy - s_y * s_y));
    
  }
  
  template <typename Iter1, typename Iter2>
  double spearmans_cc(Iter1 first1, Iter1 last1, Iter2 first2) {
    size_t n_elems = std::distance(first1, last1);
    std::vector<int> ranks_1(n_elems);
    std::vector<int> ranks_2(n_elems);
    order(first1, last1, ranks_1.begin());
    order(first2, first2 + n_elems, ranks_2.begin());
    order(ranks_1.begin(), ranks_1.end(), ranks_1.begin());
    order(ranks_2.begin(), ranks_2.end(), ranks_2.begin());
    // Pearsons of the ranks.
    return pearsons_cc(ranks_1.begin(), ranks_1.end(), ranks_2.begin());
  }
}



#endif /* _DPJ_UTILS_H_ */
