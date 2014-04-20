#ifndef DPJ_DOMAIN_SOCKET_SERVER_HH_
#define DPJ_DOMAIN_SOCKET_SERVER_HH_

#include <array>

#include <memory>
#include <functional>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace dpj { namespace net { namespace local {

  using namespace std::placeholders;
  using std::bind;
  
  using boost::system::error_code;
  
  namespace asio = boost::asio;
  using asio::local::stream_protocol;
  using asio::buffer;
  using asio::async_write;

  
  class session : public std::enable_shared_from_this<session>
  {
  public:
    session(boost::asio::io_service& io_service) : socket_{io_service} { }
    
    stream_protocol::socket& socket() { return socket_; }
    
    void start()
    { socket_.async_read_some(buffer(data_), bind(&session::read, shared_from_this(), _1, _2)); }
    
    void read(error_code const& error, size_t n)
    {
      if (!error)
        async_write(socket_, buffer(data_, n), bind(&session::write, shared_from_this(), _1));
    }
    
    void write(error_code const& error)
    {
      if (!error)
        socket_.async_read_some(buffer(data_), bind(&session::read, shared_from_this(), _1, _2));
    }
    
  private:
    stream_protocol::socket socket_;
    std::array<char, 1024> data_;
  };
  
  class server
  {
  public:
    server(boost::asio::io_service& io_service, std::string file)
    : io_service_(io_service)
    , acceptor_(io_service, stream_protocol::endpoint(file))
    {
      auto s = std::make_shared<session>(io_service_);
      acceptor_.async_accept(s->socket(), bind(&server::handle_accept, this, s, _1));
    }
    
    void handle_accept(std::shared_ptr<session> s, error_code const& error)
    {
      if (!error)
        s->start();
      
      s.reset(new session{io_service_});
      acceptor_.async_accept(s->socket(), bind(&server::handle_accept, this, s, _1));
    }
    
  private:
    asio::io_service& io_service_;
    stream_protocol::acceptor acceptor_;
  };
  
  
}
}
}

#endif /* DPJ_DOMAIN_SOCKET_SERVER_HH_ */
