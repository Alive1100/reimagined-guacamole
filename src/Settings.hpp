#pragma once

#include <cstdint>
#include <string>

struct Settings {
  std::string address;
  uint16_t port;
  std::string cert;
  std::string key;
  std::string dhparams;
};
