#pragma once

#if __has_include("modules_enabled.hpp")
#include "modules_enabled.hpp"
#endif

#if __has_include("extra_modules.hpp")
#define EXTRA_MODULES
#include "extra_modules.hpp"
#endif
