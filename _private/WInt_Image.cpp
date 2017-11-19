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

#include "WInt_Image.h"
#include "Log.h"

#include "SOIL.h"


WInt_Image::WInt_Image(const std::string &filename) :
	data(NULL),
	width(0), height(0)
{
	data = SOIL_load_image(filename.c_str(),
						   &width, &height, &nChannels,
						   SOIL_LOAD_RGBA);	// Resulting array should thusly have 4 channels
	if (!data)
		W::log << "Could not load image '" << filename << "': " << "file may not exist, or is invalid";
}

WInt_Image::~WInt_Image()
{
	if (data) free(data);
}
