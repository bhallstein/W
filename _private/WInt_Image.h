/*
 * W - a tiny 2D game development library
 *
 * ==================
 *  WInt_Image.cpp
 * ==================
 *
 * Copyright (C) 2014 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// Loads an image into a byte array w/ RGBA order
// Compatibility is a-la SOIL

#ifndef __W__WInt_Image__
#define __W__WInt_Image__

#include <string>

class WInt_Image {
public:
	WInt_Image(const std::string &filename);
	~WInt_Image();
	
	unsigned char *data;

	int width;
	int height;
	int nChannels;
	
};


#endif
