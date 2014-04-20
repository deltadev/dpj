#ifndef DPJ_DOMAIN_SOCKET_SERVER_HH_
#define DPJ_DOMAIN_SOCKET_SERVER_HH_

#include <array>

#include <memory>
#include <functional>

using namespace std::placeholders;
using std::bind;

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace dpj { namespace net { namespace local {
  
  namespace asio = boost::asio;
  using asio::local::stream_protocol;
  
  class session : public std::enable_shared_from_this<session>
  {
  public:
    session(boost::asio::io_service& io_service)
    : socket_(io_service)
    {
    }
    
    stream_protocol::socket& socket()
    {
      return socket_;
    }
    
    void start()
    {
      socket_.async_read_some(asio::buffer(data_),
                              bind(&session::handle_read, shared_from_this(), _1, _2));
    }
    
    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {
      std::cout << std::string{data_.data(), bytes_transferred} << '\n';
      if (!error)
      {
        asio::async_write(socket_, asio::buffer(data_, bytes_transferred),
                          bind(&session::handle_write, shared_from_this(), _1));
      }
    }
    
    void handle_write(const boost::system::error_code& error)
    {
      if (!error)
      {
        socket_.async_read_some(asio::buffer(data_),
                                std::bind(&session::handle_read, shared_from_this(), _1, _2));
      }
    }
    
  private:
    // The socket used to communicate with the client.
    stream_protocol::socket socket_;
    
    // Buffer used to store data received from the client.
    std::array<char, 1024> data_;
  };
  
  class server
  {
  public:
    server(boost::asio::io_service& io_service, std::string file)
    : io_service_(io_service),
    
    acceptor_(io_service, stream_protocol::endpoint(file))
    {
      auto new_session = std::make_shared<session>(io_service_);
      acceptor_.async_accept(new_session->socket(), bind(&server::handle_accept, this, new_session, _1));
    }
    
    void handle_accept(std::shared_ptr<session> new_session,
                       const boost::system::error_code& error)
    {
      if (!error)
      {
        new_session->start();
      }
      
      new_session.reset(new session(io_service_));
      acceptor_.async_accept(new_session->socket(), bind(&server::handle_accept, this, new_session, _1));
    }
    
  private:
    asio::io_service& io_service_;
    stream_protocol::acceptor acceptor_;
  };
  
  
}
}
}

#endif /* DPJ_DOMAIN_SOCKET_SERVER_HH_ */
