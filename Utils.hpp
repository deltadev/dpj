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


} // namespace dpj_utils

#endif //Utils_hpp