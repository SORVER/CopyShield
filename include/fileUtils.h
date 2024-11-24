#pragma once 

#ifdef _WIN32
#include <direct.h> // For _mkdir, _rmdir
#else
#include <sys/stat.h> // For mkdir
#include <unistd.h>   // For rmdir
#endif

// Macro definitions for creating and removing directories
#ifdef _WIN32
#define CREATE_DIR(name) _mkdir(name)
#define REMOVE_DIR(name) _rmdir(name)
#else
#define CREATE_DIR(name) mkdir(name, 0777)
#define REMOVE_DIR(name) system(("rm -rf " + string(name)).c_str()) 
#endif