#pragma once

#define FIND_CONFIG

#if __has_include("config_def.hpp")
#define CONFIG_FILE_USED "config_def.hpp"
#include "config_def.hpp"
#endif
#if __has_include("config.hpp")
#define CONFIG_FILE_USED "config.hpp"
#include "config.hpp"
#else
#warning no config.hpp file found!!
#endif

#ifndef CONFIG_FILE_USED
#error no config.hpp or config_def.hpp found!
#endif
