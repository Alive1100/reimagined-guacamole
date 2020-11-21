#pragma once
#include <boost/program_options.hpp>

#include "Settings.hpp"

boost::program_options::options_description description(Settings &settings);
