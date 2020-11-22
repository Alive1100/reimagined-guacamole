#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <functional>
#include <memory>

#include "Settings.hpp"
#include "SharedState.hpp"

class WsAcceptor : public std::enable_shared_from_this<WsAcceptor>
{
public:
  WsAcceptor(
    std::shared_ptr<boost::asio::io_context> io_context,
    boost::asio::ip::tcp::endpoint endpoint,
    const Settings &settings,
    std::shared_ptr<SharedState> state);

  void start();
  void stop();

  void accept();
  void doAccept(boost::asio::ip::tcp::socket &&socket);

  std::string getPassword() const;
private:
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ssl::context context_;

  std::shared_ptr<SharedState> state_;
};

typedef std::shared_ptr<WsAcceptor> WsAcceptorPtr;