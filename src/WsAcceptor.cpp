#include "WsAcceptor.hpp"

#include <spdlog/spdlog.h>

WsAcceptor::WsAcceptor(
        std::shared_ptr<boost::asio::io_context> ioContext,
        boost::asio::ip::tcp::endpoint endpoint,
        const Settings &settings,
        std::shared_ptr<SharedState> state)
    : acceptor_(*ioContext),
      endpoint_(endpoint),
      context_(boost::asio::ssl::context::sslv23),
      state_(state)
{
  context_.set_options(
    boost::asio::ssl::context::default_workarounds
    | boost::asio::ssl::context::no_sslv2
    | boost::asio::ssl::context::single_dh_use);
  context_.set_password_callback(std::bind(&WsAcceptor::getPassword, this));
  context_.use_certificate_chain_file(settings.cert);
  context_.use_private_key_file(settings.key, boost::asio::ssl::context::pem);
  context_.use_tmp_dh_file(settings.dhparams);
}

std::string WsAcceptor::getPassword() const
{
  return "test";
}

void WsAcceptor::start()
{
  boost::system::error_code ec;

  acceptor_.open(endpoint_.protocol(), ec);
  if (ec) {
    spdlog::error("Failed to open");
    return;
  }
  boost::asio::socket_base::reuse_address option(true);
  acceptor_.set_option(option);
  acceptor_.bind(endpoint_, ec);
  if (ec) {
    spdlog::error("Failed to bind");
    return;
  }
  acceptor_.listen(1);
  accept();
}

void WsAcceptor::stop()
{
  boost::system::error_code ec;
  acceptor_.cancel(ec);
  if (ec) {
    spdlog::error("Failed to cancel");
  }
  acceptor_.close(ec);
  if (ec) {
    spdlog::error("Failed to close");
  }
}

void WsAcceptor::accept()
{
  std::weak_ptr<WsAcceptor> ptr = shared_from_this();
  acceptor_.async_accept(
      [ptr](const boost::system::error_code &ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
          if (auto server = ptr.lock()) {
            server->doAccept(std::move(socket));
            server->accept();
          } else {
            spdlog::error("Refusing to create connection, no longer valid");
          }
        } else {
          spdlog::error("Accept failed");
        }
      });
  spdlog::info("Ready to accept");
}

void WsAcceptor::doAccept(boost::asio::ip::tcp::socket &&socket)
{
  auto connection = std::make_shared<WsConnection>(std::move(socket), context_, state_);
  connection->startTls();
}