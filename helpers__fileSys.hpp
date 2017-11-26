/*
 * W - a tiny 2D game development library
 *
 * ====================
 *  helpers__fileSys.h
 * ====================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef W_helpers__fileSys_h
#define W_helpers__fileSys_h

#include <string>

namespace W {

    bool isValidDir(const std::string &path);
    bool isValidDir(const char *path);
    bool createDir(const std::string &path);
    bool createDir(const char *path);
    
}

#endif
