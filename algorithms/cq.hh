#ifndef _DPJ_CQ_HH_
#define _DPJ_CQ_HH_

#include <vector>
#include <iterator>


template <typename T, std::size_t buf_size = 16>
class cq
{
  std::array<T, buf_size> buf{{T{}}};

  std::size_t base = 0;
  std::size_t pos  = 0;
  bool move_base = false; // Tells us whether the buffer is full.
  
public:
  cq() { }

  // Queue interface.
  //
  //   A pre-condition and post-condition of operations is
  //
  //     * base \in [0, size)
  //     * pos  \in [0, size)
  //
  //     * The current range of elements is:
  //
  //       a) [base, pos)
  //       b) [base, size) \union [0, pos)
  //
  //       The case base == pos needs to be dealt with differently
  //       in each of these cases when calling push_back.
  //
  //     * buf[pos] is the next available put slot.
  //
  void push_back(T i)
  {
    if (move_base)
      base = (base + 1) % buf_size;
    
    buf[pos] = i;
    pos = (pos + 1) % buf_size;

    if (pos == base) // We just caught up with ourselves.
      move_base = true;
  }
  T pop_front()
  {
    T r = buf[base];
    base = (base + 1) % buf_size;
    move_base = false;
    return r;
  }
  T pop_back()
  {
    pos = (pos + buf_size - 1) % buf_size;
    move_base = false;
    return buf[pos];
  }
  
  std::size_t size()
  {
    if (pos == base && move_base == false)
      return 0;
    else
      return pos > base ? pos - base : buf_size - base + pos;
  }
  
  
  // Element access
  //
  T&       front()       { return buf[base]; }
  T const& front() const { return buf[base]; }

  T&       back()        { return buf[(pos + buf_size - 1) % buf_size]; }
  T const& back()  const { return buf[(pos + buf_size - 1) % buf_size]; }


  // Iterator
  //
  // - forward_iterator_tag because, laziness.
  //
  class iterator : public std::iterator<std::input_iterator_tag, T>
  {
    T* data;
    std::size_t idx;
  public:

    iterator(T* first, std::size_t idx) : data{first}, idx{idx} { }
    iterator operator++()
    {
      idx = idx + 1;
      return {data, idx};
    }
    iterator operator++(int)
    {
      auto tmp = idx;
      operator++();
      return {data, tmp};
    }
    T& operator*() { return *(data + idx % buf_size); }
  
    bool operator!=(iterator const& x) const
    { return x.data != nullptr && data != nullptr && x.idx != idx; }
    
  };
  
  iterator begin() { return {buf.data(), base}; }
  iterator end()
  {
    if (base == pos && !move_base)
      return {nullptr, 0};
    else
      return {buf.data(), base + size()};
  }
  
};



#endif /* _DPJ_CQ_HH_ */
