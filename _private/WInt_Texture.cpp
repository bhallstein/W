/*
 * W - a tiny 2D game development library
 *
 * ====================
 *  WInt_Texture.cpp
 * ====================
 *
 * Copyright (C) 2014 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "WInt_Texture.h"
#include "WInt_Image.h"
#include "../types.h"
#include "../Log.h"

#include "oglInclude.h"
#include "SOIL.h"

WInt_Texture::WInt_Texture(WInt_Image *image) :
	glTexID(0)
{
	obtainOGLTexID();
	if (image) upload(image);
}

WInt_Texture::~WInt_Texture()
{
	glDeleteTextures(1, &glTexID);
}

void WInt_Texture::upload(WInt_Image *image) {
	width = image->width;
	height = image->height;
	
	SOIL_create_OGL_texture(image->data,
							image->width,
							image->height,
							image->nChannels,
							glTexID,
							0);
	// Set linear filtering
	bind();
	setFiltering(WInt_Filter_Linear);
	unbind();
}

void WInt_Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, glTexID);
}
void WInt_Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void WInt_Texture::setFiltering(WInt_Filtering_T f) {
	if (f == WInt_Filter_Linear) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (f == WInt_Filter_Nearest) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

void WInt_Texture::obtainOGLTexID() {
	if (glTexID) return;
	glGenTextures(1, &glTexID);
}
