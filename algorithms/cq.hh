#ifndef _DPJ_CQ_HH_
#define _DPJ_CQ_HH_

#include <vector>

// TODO: template on type and size of underlying array.
//
class cq
{
  static const std::size_t size = 10;
  std::array<int, size> buf;

  std::size_t base;
  std::size_t pos;
public:
  typedef int value_type;

  cq()
  {
    buf.fill(0);
    base = 0;
    pos = 0;
  }
  
  // pre-condition and post-condition of operations is
  //
  //   * base \in [0, size)
  //   * pos  \in [0, size)
  //   * buf[pos] is the next available put slot.
  //
  void push_back(int i)
  {
    buf[pos] = i;
    pos = (pos + 1) % size;
    if (pos == base)
      pop_front();
  }
  int pop_front()
  {
    int r = buf[base];
    base = (base + 1) % size;
    return r;
  }
  int& front() { return buf[base]; }
  int const& front() const { return buf[base]; }
  int & back() { return buf[(pos + size - 1) % size]; }
  int const& back() const { return buf[(pos + size - 1) % size]; }

  struct iterator
  {
    int* first;
    std::size_t idx;
    iterator(int* first, std::size_t idx) : first(first), idx(idx) { }
    iterator operator++()
    {
      idx = (idx + 1) % cq::size;
      return iterator(first, idx);
    }
    iterator operator++(int)
    {
      auto tmp = idx;
      operator++();
      return iterator(first, tmp);
    }
    int& operator*() { return *(first + idx); }
  
    bool operator==(iterator const& other) const { return other.idx == idx; }
    bool operator!=(iterator const& other) const { return other.idx != idx; }
  };

  
  iterator begin() { return iterator(&buf[base], base); }
  iterator end() { return iterator(&buf[pos], pos); }
  
  typedef iterator iterator;
};



#endif /* _DPJ_CQ_HH_ */
