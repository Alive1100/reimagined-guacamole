#include <cstdlib>
#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>

#include "Settings.hpp"
#include "SettingsDescription.hpp"
#include "SharedState.hpp"
#include "WsAcceptor.hpp"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  Settings settings;

  po::options_description desc("Options");
  desc.add_options()("help", "Display help message");

  desc.add(description(settings));

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }

  auto ioContext = std::make_shared<boost::asio::io_context>();

  auto address = boost::asio::ip::make_address(settings.address);
  spdlog::info("Server address: {}:{}", settings.address, settings.port);

    auto sharedState = std::make_shared<SharedState>();
  auto acceptor = std::make_shared<WsAcceptor>(
      ioContext,
      boost::asio::ip::tcp::endpoint(address, settings.port),
      sharedState);

  acceptor->start();

  ioContext->run();

  return EXIT_SUCCESS;
}
