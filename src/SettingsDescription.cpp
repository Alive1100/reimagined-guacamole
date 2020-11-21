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
      ("port,p", po::value<uint16_t>(&settings.port)->default_value(10101), "Port");
  // clang-format on

  return desc;
}
