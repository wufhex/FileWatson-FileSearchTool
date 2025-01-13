#pragma once

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 1

#define VERSION_INT ((VERSION_MAJOR * 10000) + (VERSION_MINOR * 100) + VERSION_PATCH)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define VERSION_STRING TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH)