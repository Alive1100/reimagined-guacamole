#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>

class SharedState;

class WsConnection : public std::enable_shared_from_this<WsConnection>
{
public:
  WsConnection(
    boost::asio::ip::tcp::socket&& socket,
    std::shared_ptr<SharedState> state);
  ~WsConnection();
  void start();
  void hello(); // sends the session number to use as identity
  void read();
  void write(const std::string &data);
  void consume(const std::size_t bytes);

protected:
  virtual void doRead(const std::size_t bytesRead);
  virtual void handleError(const boost::system::error_code& ec);

protected:
  boost::beast::flat_buffer buffer_;

private:
  boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
  std::shared_ptr<SharedState> state_;
};

typedef std::shared_ptr<WsConnection> WsConnectionPtr;