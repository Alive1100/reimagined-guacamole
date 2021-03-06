#include "WsConnection.hpp"

#include <spdlog/spdlog.h>

#include <iostream>
#include <sstream>

#include "SharedState.hpp"

WsConnection::WsConnection(
    boost::asio::ip::tcp::socket&& socket,
    boost::asio::ssl::context& context,
    std::shared_ptr<SharedState> state)
  : ws_(std::move(socket), context)
  , state_(state)
{
  state_->add(this);
}

WsConnection::~WsConnection()
{
  state_->remove(this);
}

void WsConnection::startTls()
{
  ws_.next_layer().async_handshake(
    boost::asio::ssl::stream_base::server,
    [connection = shared_from_this()](const boost::system::error_code& ec) {
      if (ec) {
        spdlog::error("Failed tls handshake: {}", ec.message());
      } else {
        connection->onTlsHandshake();
      }
    }
  );
}

void WsConnection::onTlsHandshake()
{
  start();
}

void WsConnection::start()
{
  // TODO - "correct/better?" use of beast's buffers/front handlers
  ws_.async_accept([connection = shared_from_this()](const boost::system::error_code& ec) {
    if (!ec) {
      connection->hello();
      connection->read();
    } else {
      spdlog::error("Websocket handshake failed: {}", ec.message());
    }
  });
}

void WsConnection::hello()
{
  std::stringstream ss;
  ss << "{ \"id\": \"" << state_->getSessionNumber(this) << "\" }";
  write(ss.str());
}

void WsConnection::read()
{
  ws_.async_read(buffer_,
  [connection = shared_from_this()](
    const boost::system::error_code& ec, const std::size_t bytesRead) {
    if (ec) {
      connection->handleError(ec);
    } else {
      connection->doRead(bytesRead);
    }
  });
}

void WsConnection::write(const std::string &data)
{
  ws_.write(boost::asio::buffer(data.data(), data.size()));
}

void WsConnection::consume(const std::size_t bytes)
{
  buffer_.consume(bytes);
}

void WsConnection::doRead(const std::size_t bytesRead)
{
  std::string message = boost::beast::buffers_to_string(buffer_.data());
  consume(bytesRead);
  state_->send(this, message);
  read();
}

void WsConnection::handleError(const boost::system::error_code& ec)
{
  spdlog::error("error: {}", ec.message());
}