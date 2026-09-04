#pragma once

#if __has_include("config_def.h")
#define CONFIG_FILE_USED "config_def.h"
#include "config_def.h"
#endif
#if __has_include("config.h")
#define CONFIG_FILE_USED "config.h"
#include "config.h"
#endif

#ifndef CONFIG_FILE_USED
#error no config.h or config_def.h found!
#endif
