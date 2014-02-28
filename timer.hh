#ifndef DPJ_TIMER_HH
#define DPJ_TIMER_HH

namespace dpj
{
  namespace ch = std::chrono;

  template<typename Derived, typename Duration>
  class timer_base
  {
  protected:
    typedef ch::high_resolution_clock clk_t;
    typedef ch::time_point<clk_t> tp_t;

    std::string msg;
    std::ostream os;
    tp_t t0, t1;
    bool stopped = false;
  public:
    
    
    bool dtor_prints = true;
    
    timer_base(std::string msg = "", std::ostream& os = std::cerr)
      : msg{msg}
      , os{os.rdbuf()}
      { t0 = clk_t::now(); t1 = t0; }

    Duration elapsed()
    {
      return stopped ? ch::duration_cast<Duration>(t1 - t0)
                     : ch::duration_cast<Duration>(clk_t::now() - t0);
    }

    
    void stop() { t1 = clk_t::now(); stopped = true; }
    void reset() { t0 = clk_t::now(); t1 = t0; stopped = true; }
    void start() { t0 += clk_t::now() - t1; stopped = false; }


    ~timer_base() { if (dtor_prints) static_cast<Derived&>(*this).print(); }
  };
  
  class timer_s : public timer_base<timer_s, ch::seconds>
  {
    typedef timer_base<timer_s, ch::seconds> base;
  public:
    timer_s(std::string msg) : base::timer_base(msg) { }
    void print() { os << msg << ": " << elapsed().count() << "s" << '\n'; }
  };

  class timer_ms : public timer_base<timer_ms, ch::milliseconds>
  {
    typedef timer_base<timer_ms, ch::milliseconds> base;
  public:
    timer_ms(std::string msg) : base::timer_base(msg) { }
    void print() { os << msg << ": " << elapsed().count() << "ms" << '\n'; }
  };

  class timer_us : public timer_base<timer_us, ch::microseconds>
  {
    typedef timer_base<timer_us, ch::microseconds> base;
  public:
    timer_us(std::string msg) : base::timer_base(msg) { }
    void print() { os << msg << ": " << elapsed().count() << "us" << '\n'; }
  };
  
  class timer_ns : public timer_base<timer_ns, ch::nanoseconds>
  {
    typedef timer_base<timer_ns, ch::nanoseconds> base;
  public:
    timer_ns(std::string msg) : base::timer_base(msg) { }
    void print() { os << msg << ": " << elapsed().count() << "ns" << '\n'; }
  };

} // namespace dpj


#endif /* DPJ_TIMER_HH */
