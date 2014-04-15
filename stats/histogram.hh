#ifndef DPJ_HISTOGRAM_HH_
#define DPJ_HISTOGRAM_HH_

#include <cmath>
#include <vector>
#include <algorithm>

namespace dpj
{
  using std::begin; using std::end;

  class histogram_t
  {
  public:
    std::vector<double> data;
    std::vector<double> bins; // right hand end points
    std::vector<int>    counts;

    struct pars
    {
      pars() { }
      pars(bool) : defaults{true} { }

      double min       = 0;
      double max       = 0;
      int num_bins     = 50;
      bool defaults    = false;
      bool log_counts  = false;
    };
  
    template<typename Iterator>
    histogram_t(Iterator b, Iterator e) : data{b, e} { }
  
    template<typename Container, typename Accessor>
    histogram_t(Container c, Accessor a)
      {
        data.reserve(c.size());
        for (auto const& e : c)
          data.emplace_back(a(e));
      }
    
    friend
    void bin(histogram_t& h, histogram_t::pars pars = true)
      {
        if (h.data.empty())
          std::runtime_error("Error: trying to build histogram with nodata!");
    
        // Remove any NaNs
        auto er = std::remove_if(begin(h.data), begin(h.data), 
                                 [](double d) { return std::isnan(d); });
        h.data.erase(er, end(h.data));    

        std::sort(begin(h.data), end(h.data));

        if (pars.defaults == true)
        {
          pars.min = h.data.front();
          pars.max = h.data.back();
        }    

        double range = pars.max - pars.min;
        double eps = std::numeric_limits<double>::epsilon();
        double bin_width = (range + pars.num_bins * eps) / static_cast<double>(pars.num_bins);

        h.bins.push_back(pars.min + bin_width);
        for (int i = 1; i < pars.num_bins; ++i)
          h.bins.push_back(h.bins.back() + bin_width);

        // Main loop over sorted data.
        //
        auto it = begin(h.data);
        auto e = end(h.data);
        for (auto a : h.bins)
        {
          auto tmp = std::upper_bound(it, e, a);
          auto count = std::distance(it, tmp);
          if (pars.log_counts)
            h.counts.push_back(::log2(count));
          else
            h.counts.push_back(count);
          it = tmp;
        }
      }
    
    friend
    void draw(histogram_t const& h, unsigned rows = 25, std::ostream& os = std::cout)
      {
        auto max_count = *std::max_element(begin(h.counts), end(h.counts));
        auto num_bins = h.bins.size();
        std::string grid(rows * h.bins.size(), ' ');
        for (int idx =0; idx != rows * num_bins; ++idx) 
        {
          int col = idx % num_bins;
          int row = idx / num_bins;
          if (h.counts[col] * rows / max_count >= rows - row)
            grid[idx] = '*';
        }
        for (int i = 0; i < rows; ++i) 
        {
          for (int j = 0; j < num_bins; ++j)
            os << grid[i * num_bins + j];
          os << '\n';
        }   
      }
  
    friend
    void print(histogram_t const& h, std::ostream& os = std::cout)
      {
        for (unsigned i = 0; i < h.bins.size(); ++i)
          os << h.bins[i] << ' ' << h.counts[i] << '\n';
      }
  };
}
#endif /* DPJ_HISTOGRAM_HH_ */
