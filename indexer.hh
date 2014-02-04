#ifndef _INDEXER_H_
#define _INDEXER_H_

#include <iterator>

namespace dpj {

  // http://stackoverflow.com/a/10962575/710408
  //
  
  template <typename T>
  struct iterator_extractor { typedef typename T::iterator type; };
  
  template <typename T>
  struct iterator_extractor<T const> { typedef typename T::const_iterator type; };
  
  
  template <typename T>
  class indexer {
  public:
    class iterator {
      typedef typename iterator_extractor<T>::type inner_iterator;
      
      typedef typename std::iterator_traits<inner_iterator>::reference inner_reference;
    public:
      typedef std::pair<size_t, inner_reference> reference;
      
      iterator(inner_iterator it): pos_(0), it_(it) {}
      
      reference operator*() const { return reference(pos_, *it_); }
      
      iterator& operator++() { ++pos_; ++it_; return *this; }
      iterator operator++(int) { iterator tmp(*this); ++*this; return tmp; }
      
      bool operator==(iterator const& it) const { return it_ == it.it_; }
      bool operator!=(iterator const& it) const { return !(*this == it); }
      
    private:
      size_t pos_;
      inner_iterator it_;
    };
    
    indexer(T& t): container_(t) {}
    
    iterator begin() const { return iterator(container_.begin()); }
    iterator end() const { return iterator(container_.end()); }
    
  private:
    T& container_;
  }; // class indexer
  
  template <typename T>
  indexer<T> index(T& t) { return indexer<T>(t); }
}

#endif /* _INDEXER_H_ */
