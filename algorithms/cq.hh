#ifndef _DPJ_CQ_HH_
#define _DPJ_CQ_HH_

#include <array>
#include <iterator>

/// circular queue of length that allows an eficient mod operation.
//
template <typename T, uint32_t exponent = 6>
class cq
{
  static constexpr uint32_t buf_size = 1 << exponent;
  static constexpr uint32_t mod_mask = (buf_size - 1);
  
  std::array<T, buf_size> buf;
  
  uint32_t base = 0;
  uint32_t pos  = 0;
  bool full_    = false;
  
public:

  cq() { }
  
  template<typename It>
  cq(It first, It last)
  {
    while(first++ != last)
      push_back(*first);
  }
  
  void push_back(T const& i)
  {
    buf[pos] = i;
    pos = (pos + 1) & mod_mask;
    
    if (full_)
      base = (base + 1) & mod_mask;
    
    if (pos == base) // We just caught up with ourselves.
      full_ = true;
  }
  void push_back(T&& i)
  {
    buf[pos] = std::move(i);
    pos = (pos + 1) & mod_mask;
    
    if (full_)
      base = (base + 1) & mod_mask;
    
    if (pos == base)
      full_ = true;
  }
  void pop_front()
  {
    full_ = false;
    base = (base + 1) & mod_mask;
  }
  void pop_back()
  {
    full_ = false;
    pos = (pos - 1) & mod_mask;
  }
  
  int size() const
  {
    if (pos == base)
      return full_ ? buf_size : 0;
    else
      return pos > base ? pos - base : buf_size - base + pos;
  }

  void clear() { base = pos = 0; full_ = false; }
  
  bool full() { return full_; }
  bool empty() { return pos == base && !full(); }

  T&       front()       { return buf[base]; }
  T const& front() const { return buf[base]; }

  T&       back()        { return buf[(pos - 1) & mod_mask]; }
  T const& back()  const { return buf[(pos - 1) & mod_mask]; }

  // forward_iterator_tag, because laziness. It's main use is to
  // allow printin the queue in a for( : ) loop.
  //
  class iterator : public std::iterator<std::input_iterator_tag, T>
  {
    T const* data;
    uint32_t idx;
  public:

    iterator(T const* first, uint32_t idx) : data{first}, idx{idx} { }
    iterator operator++()
    {
      idx = idx + 1;
      return *this;
    }
    iterator operator++(int)
    {
      auto tmp = *this;
      operator++();
      return tmp;
    }
    T const& operator*() { return *(data + (idx & mod_mask)); }
  
    // Continues iteration so long as the iterators are valid and not equal.
    bool operator!=(iterator const& x) const
    { return x.data != nullptr && data != nullptr && x.idx != idx; }
  };
  
  iterator begin() const
  {
    if (base == pos && !full_)
      return {nullptr, 0};
    else
      return {buf.data(), base};
  }
  iterator end() const
  {
    if (base == pos && !full_)
      return {nullptr, 0};
    else
      return {buf.data(), base + size()};
  }
};

template<typename T, uint32_t size>
std::ostream& operator<<(std::ostream& os, cq<T, size> const& q)
{
  for (auto const& el : q) os << el << ' ';
  return os;
}

#endif /* _DPJ_CQ_HH_ */
