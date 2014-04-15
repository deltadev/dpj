#include <random>

#include "histogram.hh"

bool histogram_test();
bool covariance_test();
bool pearson_cc_test();
bool spearman_cc_test();

bool histogram_test()
{
  std::mt

  dpj::histogram_t<double> h(begin(rs), end(rs));

  bin(h);
  draw(h);
  print(h);

  return true;
}

bool covariance_test() {
  double res = dpj::covariance(x.begin(), x.end(), y.begin());
  double actual = -0.03959601;
  std::cout << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}

bool pearson_cc_test() {
  double res = dpj::pearsons_cc(x.begin(), x.end(), y.begin());
  double actual = -0.38256335;
  std::cout << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}

bool spearman_cc_test() {
  double res = dpj::spearmans_cc(x.begin(), x.end(), y.begin());
  double actual = -0.34923076923076929;
  std::cout << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}
