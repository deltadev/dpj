#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_




#include <cmath>

struct maybe_is_nan
{
  template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type>
  bool operator()(T f) { return std::isnan(f); }
  template <typename T>
  bool operator()(T f) { return false;}
};

template <typename T>
class Histogram
{
  typename std::vector<T> m_data;
  typename std::vector<double> m_rightHandEndPoints;
  unsigned m_numBins;
  double m_min, m_max, m_binWidth;
  std::vector<unsigned> m_counts;
  
public:
  
  template<typename Iterator>
  Histogram(Iterator b, Iterator e) : m_data(b, e), m_numBins(10) { }
  
  template<typename Container, typename Accessor>
  Histogram(Container c, Accessor a) : m_numBins(c.size())
  {
    for (auto const& e : c)
      m_data.push_back(a(e));
  }
  
  void numBins(unsigned numBins) { m_numBins = numBins; }
  
  double binWidth() const { return m_binWidth; }
  unsigned numBins() const { return m_numBins; }
  void binData()
  {
    if (m_data.empty())
    {
      std::cerr << "Error: trying to build histogram with nodata!. Exiting...\n";
      exit(EXIT_FAILURE);
    }
    
    // Remove any NaNs
    m_data.erase(std::remove_if(m_data.begin(), m_data.end(), maybe_is_nan()), m_data.end());
    
    std::sort(m_data.begin(), m_data.end());
    m_min = m_data.front();
    m_max = m_data.back();
    m_binWidth = (m_max - m_min + m_numBins * std::numeric_limits<double>::epsilon()) / static_cast<double>(m_numBins);
    
    m_rightHandEndPoints.push_back(m_min + m_binWidth);
    for (unsigned i = 1; i < m_numBins; ++i)
    {
      m_rightHandEndPoints.push_back(m_rightHandEndPoints.back() + m_binWidth);
    }
    //
    // Main loop over sorted data.
    //
    auto it = m_data.begin();
    auto end = m_data.end();
    for (auto a : m_rightHandEndPoints)
    {
      auto tmp = std::upper_bound(it, end, a);
      m_counts.push_back((unsigned)std::distance(it, tmp));
      it = tmp;
    }
  }
  void draw(unsigned rows = 25) const {
    unsigned max_count = maxCount();
    std::string grid(rows * numBins(), ' ');
    for (unsigned idx(0); idx != rows * numBins(); ++idx) {
      unsigned col = idx % numBins();
      unsigned row = idx / numBins();
      if (m_counts[col] * rows / (double)max_count >= rows - row) {
        grid[idx] = '*';
      }
    }
    for (unsigned i(0); i < rows; ++i) {
      for (unsigned j(0); j < m_numBins; ++j) {
        std::cout << grid[i * m_numBins + j];
      }
      std::cout << '\n';
    }
    
  }
  unsigned maxCount() const { return *std::max_element(m_counts.begin(), m_counts.end());}
  
  std::vector<unsigned> const& counts() const { return m_counts; }
  
  std::vector<double> const& endPoints() const {return m_rightHandEndPoints; }
  
  double max() const { return m_max; }
  double min() const { return m_min; }
  
  void summary(std::ostream& os = std::cout) const
  {
    os << "min value: " << m_min << '\n';
    os << "max value: " << m_max << '\n';
    os << "num bins : " << m_numBins << '\n';
    os << "bin width: " << m_binWidth << '\n';
    os << "rightHandEndpoint count\n";
    for (unsigned i = 0; i < m_rightHandEndPoints.size(); ++i)
    {
      os << m_rightHandEndPoints[i] << ' ' << m_counts[i] << '\n';
    }
  }
};
#endif /* _HISTOGRAM_H_ */
