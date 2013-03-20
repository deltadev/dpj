#ifndef Utils_hpp
#define Utils_hpp

#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

#include <unordered_set>

namespace dpj_utils {
  
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
  
  
} // namespace dpj_utils

#endif //Utils_hpp
