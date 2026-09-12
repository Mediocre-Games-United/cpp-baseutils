#pragma once

#define CMP_NAME "mgu"
#define APP_NAME "<no app name found, define APP_NAME in config.h!>"
#define APP_NAME_FULL "<no full app name found, define APP_NAME_FULL in config.h!>"

#define APP_ID          (CMP_NAME ":" APP_NAME)

#define STUDIO_NAME "Mediocre Games United"

#define INIT_CANVAS_W 800
#define INIT_CANVAS_H 600

#ifndef VERSION_MAJOR
#define VERSION_MAJOR 0
#endif
#ifndef VERSION_MINOR
#define VERSION_MINOR 1
#endif
#ifndef VERSION_PATCH
#define VERSION_PATCH 0
#endif
