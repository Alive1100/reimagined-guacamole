#include <boost/program_options.hpp>

#include "SettingsDescription.hpp"

namespace po = boost::program_options;

po::options_description description(Settings &settings)
{
  po::options_description desc("Server options");

  // clang-format off
  desc.add_options()
      ("address,a", po::value<std::string>(&settings.address)->default_value("127.0.0.1"),
       "Bind address")
      ("port,p", po::value<uint16_t>(&settings.port)->default_value(8443), "Port")
      ("cert,c", po::value<std::string>(&settings.cert)->default_value("server.crt"), "Server certificate")
      ("key,k", po::value<std::string>(&settings.key)->default_value("server.key"), "Server key")
      ("dhparams,d", po::value<std::string>(&settings.dhparams)->default_value("dhparams.pem"), "DH param file");
  // clang-format on

  return desc;
}
