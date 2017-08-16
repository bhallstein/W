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

// An OGL texture
// Upload from a WInt_Image
// If an Image is supplied, is uploaded immediately
// Can also call upload() manually to upload anew, e.g. after modifying

#ifndef __W__WInt_Texture__
#define __W__WInt_Texture__


class WInt_Image;


enum WInt_Filtering_T {
	WInt_Filter_Linear,
	WInt_Filter_Nearest
};


class WInt_Texture {
public:
	WInt_Texture(WInt_Image *im = 0);
	~WInt_Texture();
	
	int width, height;
	
	void upload(WInt_Image *);
	void bind();
	void setFiltering(WInt_Filtering_T);
		// Does not bind; must do so before calling.
		// Def. is linear.
	
	static void unbind();
	
private:
	unsigned int glTexID;
	
	void obtainOGLTexID();
	
};

#endif
