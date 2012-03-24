#ifndef Utils_hpp
#define Utils_hpp

#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

namespace dpj_utils {

template <typename T> struct printPair
{ void operator()(T t) { std::cout << t.first << ' ' << t.second << '\n'; } };

template <typename VectorType>
void printVector(const VectorType& pload, size_t n = 10, std::ostream& os = std::cout)
{
    typedef typename VectorType::value_type value_type;
    n = std::min(n, pload.size());

    if (n > 10) 
    {
        //
        // Print an abridged version.
        // 
        std::copy(pload.begin(), pload.begin() + n / 2,
                  std::ostream_iterator<value_type >(os, "\n"));
        
        std::cout << "\n...\n...\n...\n\n" << std::endl;   

        std::copy(pload.rbegin(), pload.rbegin() + n,
                  std::ostream_iterator<value_type >(os, "\n"));
        os << std::endl;
    }
    else
    {
        std::copy(pload.begin(), pload.end(), std::ostream_iterator<value_type >(os, "\n"));
    }
}
//
// order - obtain a rank or order of a container.
//
//     If the elements in the container are 11, 14, 12, 15, 13, then the order
//     is the sequence: 0 2 4 1 3 and the rank is 0 3 1 4 2. The rank is
//     obtained as the order of the order.
//     The order tells elements where to go, the rank tells them where they are!
// 
// constructor
//     pass the container for which the order is wanted.
//
// index
//     returns a ref to the ordering.

template <typename Container>
class Order
{
  public:
    typedef typename Container::value_type value_type;
    typedef std::pair<value_type, unsigned> Pair;

    Order(Container const& c)
        : m_begin(std::begin(c)), m_end(std::end(c)) { setup(); }

    std::vector<unsigned> const& index() { return m_index; }

  private:    
    void setup()
    {
        auto it = m_begin;
        for (unsigned i = 0; it != m_end; ++it, ++i) 
        {
            m_ps.push_back(Pair(*it, i));
        }
        std::sort(m_ps.begin(), m_ps.end());
        m_index.reserve(std::distance(m_begin, m_end));
        for (auto const& p : m_ps)
        {
            m_index.push_back(std::get<1>(p));
        }
    }
    typename Container::const_iterator m_begin;
    typename Container::const_iterator m_end;
    std::vector<Pair> m_ps;
    std::vector<unsigned> m_index;
};

    
    template <typename T>
    class Histogram
    {
        typename std::vector<T> m_data;
        typename std::vector<T> m_rightHandEndPoints;
        unsigned m_numBins;
        T m_min, m_max, m_binWidth;
        std::vector<unsigned> m_counts;
        
    public:
        
        template<typename Iterator>
        Histogram(Iterator b, Iterator e) : m_data(b, e), m_numBins(10) { }
        
        void numBins(unsigned numBins) { m_numBins = numBins; }
        T binWidth() { return m_binWidth; }
        unsigned numBins() { return m_numBins; }
        void binData()
        {
            if (m_data.empty())
            {
                std::cerr << "Error: trying to build histogram with nodata!. Exiting...\n";
                exit(EXIT_FAILURE);
            }
            std::sort(m_data.begin(), m_data.end());
            m_min = m_data.front();
            m_max = m_data.back();
            m_binWidth = (m_max - m_min) / static_cast<T>(m_numBins);
            
            // This makes sure the final bin captures the largest value after rounding errors.
            // It is also a dumb way to do it. Sorry.
            while (m_min + m_binWidth * m_numBins < m_max) 
            {
                m_binWidth += std::numeric_limits<T>::epsilon() * m_numBins;
            }
            
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
        unsigned maxCount() { return *std::max_element(m_counts.begin(), m_counts.end());}
        
        std::vector<unsigned> const& counts() { return m_counts; }
        
        std::vector<T> const& endPoints() {return m_rightHandEndPoints; }
        
        T max() { return m_max; }
        T min() { return m_min; }

        void summary(std::ostream& os = std::cout)
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

} // namespace dpj_utils

#endif //Utils_hpp
