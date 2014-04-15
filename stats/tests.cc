#include <array>
#include <map>
#include <string>

#include <random>

#include "histogram.hh"
#include "stats.hh"

bool covariance_test();
bool pearson_cc_test();
bool spearman_cc_test();

namespace
{
  template <typename T>
  void run_test(T t)
  { std::cout << t.first << ", test pass:\n" << std::boolalpha << t.second() << '\n'; }
  
  bool histogram_test_normal()
  {
    std::vector<double> data(10000);
    std::normal_distribution<> dist;
    std::mt19937 rng{static_cast<unsigned>(clock())};
    std::generate(begin(data), end(data), [&]() { return dist(rng); });
    dpj::histogram_t h(begin(data), end(data));
    bin(h);
    draw(h);
    print(h);
    
    return true;
  }
  
  bool histogram_test_uniform()
  {
    std::vector<double> data(10000);
    std::uniform_real_distribution<> dist;
    std::mt19937 rng{static_cast<unsigned>(clock())};
    std::generate(begin(data), end(data), [&]() { return dist(rng); });
    dpj::histogram_t h(begin(data), end(data));
    dpj::histogram_t::pars pars;
    pars.min = -1;
    pars.max = 2;
    pars.num_bins = 30;
    bin(h, pars);
    draw(h);
    print(h);
    
    return true;
  }
  
  bool histogram_test_poisson()
  {
    std::vector<double> data(10000);
    std::poisson_distribution<> dist{4};
    std::mt19937 rng{static_cast<unsigned>(clock())};
    std::generate(begin(data), end(data), [&]() { return dist(rng); });
    dpj::histogram_t h(begin(data), end(data));
    dpj::histogram_t::pars pars;
    pars.min = 0;
    pars.max = 15;
    pars.num_bins = 10;
    bin(h, pars);
    draw(h);
    print(h);
    
    return true;
  }
  
  std::array<double, 25> x
  {
    {
      0.95066605,  0.73844443,  0.25084646,  0.48559874,  0.7162443 ,
      0.93823069,  0.00725236,  0.43974925,  0.63150104,  0.8168913 ,
      0.20459832,  0.99897657,  0.60544523,  0.50133834,  0.79542532,
      0.32799355,  0.07951997,  0.70075721,  0.8468032 ,  0.08510824,
      0.64527051,  0.37971715,  0.61769385,  0.03528504,  0.41282303
    }
  };
  std::array<double, 25> y
  {
    {
      0.16297828, 0.48221679, 0.84467651, 0.84281474, 0.08119215,
      0.77225741, 0.43086105, 0.15751603, 0.02472734, 0.10697636,
      0.98900186, 0.20101628, 0.11519468, 0.58241635, 0.43585047,
      0.60467086, 0.99328387, 0.99207509, 0.31556858, 0.2553983 ,
      0.51795227, 0.00341642, 0.17557971, 0.8360058 , 0.98655175
    }
  };
}
int main()
{
  std::map<std::string, std::function<bool()>> tests
  {
    {"histogram_test_normal", histogram_test_normal},
    {"histogram_test_uniform", histogram_test_uniform},
    {"histogram_test_poisson", histogram_test_poisson},
    {"covariance_test", covariance_test},
    {"pearson_cc_test", pearson_cc_test},
    {"spearman_cc_test", spearman_cc_test},
  };
  
  
  for (auto t : tests)
    run_test(t);
};

bool covariance_test()
{
  double res = dpj::covariance(x.begin(), x.end(), y.begin());
  double actual = -0.03959601;
  std::cerr << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}

bool pearson_cc_test()
{
  double res = dpj::pearsons_cc(x.begin(), x.end(), y.begin());
  double actual = -0.38256335;
  std::cerr << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}

bool spearman_cc_test()
{
  double res = dpj::spearmans_cc(x.begin(), x.end(), y.begin());
  double actual = -0.34923076923076929;
  std::cerr << actual << ", " << res << '\n';
  return std::abs(res - actual) <  0.0000001;
}
