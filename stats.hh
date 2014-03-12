#ifndef DPJ_STATS_H_
#define DPJ_STATS_H_

#include <vector>
#include <cmath>

namespace dpj 
{  
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


#endif /* DPJ_STATS_H_ */
