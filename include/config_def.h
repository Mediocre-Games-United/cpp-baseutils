#pragma once

#ifndef FIND_CONFIG
#error Please include find_config.h instead!
#endif

#define CMP_NAME "mgu"
#define APP_NAME "<no app name found, define APP_NAME in config.h!>"
#define APP_NAME_FULL "<no full app name found, define APP_NAME_FULL in config.h!>"

#define APP_ID          (CMP_NAME ":" APP_NAME)

#define STUDIO_NAME "Mediocre Games United"
#define ENGINE_NAME "Made with S2D"

#ifndef GIT_COMMIT
#define GIT_COMMIT "unknown"
#endif
#ifndef BUILD_TIME
#define BUILD_TIME "unknown date"
#endif

#define BUILD_STRING (GIT_COMMIT "@" BUILD_TIME)


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

#ifdef EDITOR
#define VERSION_TYPE "editor"
#else
#ifdef RELEASE
#define VERSION_TYPE "prod"
#else
#ifdef BETA
#define VERSION_TYPE "beta"

#endif
#endif
#endif

#ifndef VERSION_TYPE
#define VERSION_TYPE "standard"
#endif

#define STR_X(v) #v
#define STR(v) STR_X(v)
#define VERSION_STRING ("v" STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH) "-" VERSION_TYPE)

#ifndef COLLISION_CELL_SIZE
#define COLLISION_CELL_SIZE 1024
#endif
#ifndef UNIT_SCALE
#define UNIT_SCALE 64.0
#endif
