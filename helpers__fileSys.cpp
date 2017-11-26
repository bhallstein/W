/*
 * W - a tiny 2D game development library
 *
 * ======================
 *  helpers__fileSys.cpp
 * ======================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "helpers__fileSys.hpp"
#include "types.h"

#ifdef WTARGET_MAC
    #include <Cocoa/Cocoa.h>
#elif defined WTARGET_IOS
    #include <Foundation/Foundation.h>
#elif defined WTARGET_WIN
    #include "Windows.h"
#endif

bool W::isValidDir(const std::string &path) {
    return isValidDir(path.c_str());
}
bool W::isValidDir(const char *path) {
    #ifdef __APPLE__
        BOOL isdir;
        [[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithCString:path encoding:NSUTF8StringEncoding]
                                             isDirectory:&isdir];
        return isdir;
    #elif defined _WIN32 || _WIN64
        DWORD dw = GetFileAttributes(path);
        return (dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_DIRECTORY));
    #endif
}
bool W::createDir(const std::string &path) {
    return createDir(path.c_str());
}
bool W::createDir(const char *dir) {
    #ifdef __APPLE__
        return [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithCString:dir encoding:NSUTF8StringEncoding]
                                         withIntermediateDirectories:YES
                                                          attributes:nil
                                                               error:nil];
    #elif defined _WIN32 || _WIN64
        return CreateDirectory(dir, NULL);
    #endif
}

