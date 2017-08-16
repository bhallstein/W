#include "types.h"
#include "W.h"

#include "SOIL.h"

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#elif defined WIN32 || WIN64
	#include <gl\gl.h>
	#include <gl\glu.h>
#endif

#define NCHANNELS 4		// Textures are uploaded as RGBA

inline int toPow2(int x)
{
    int z = 1;
    while (z < x) z = z << 1;
    return z;
}


W::Texture *W::Texture::_placeholderTexture = NULL;

W::Texture::Texture(const std::string &_filename) :
	usageCount(0), filename(_filename), _uploaded(false)
{
	using std::string;
	
	// Load image into data array
	int imChannels;
	unsigned char *imagedata = SOIL_load_image(
		filename.c_str(),
		&imageWidth, &imageHeight, &imChannels,
		SOIL_LOAD_RGBA	// Resulting array should thusly have 4 channels
	);
	if (!imagedata) {
		string msg =
			string("Could not load image '") + filename + string("': ") +
			string("the file may not exist, or is not a valid image");
		throw Exception(msg);
	}
	
	// Copy into buffer with power-of-2-sized edges
	textureWidth  = toPow2(imageWidth);
	textureHeight = toPow2(imageHeight);
	p2imagedata = (unsigned char *) malloc(textureWidth * textureHeight * NCHANNELS * sizeof(unsigned char));
	if (!p2imagedata)
		throw Exception("Memory allocation error");
	for (int y=0; y < imageHeight; y++)
		memcpy(
			p2imagedata + y*textureWidth * NCHANNELS,
			imagedata + y*imageWidth * NCHANNELS,
			imageWidth * NCHANNELS
		);
	free(imagedata);
	
	_lock_mutex(&texture_mutex);
	_textures_to_upload.push_back(this);
	_unlock_mutex(&texture_mutex);
}
W::Texture::Texture(unsigned int _glTexId, size _imsize, size _texsize) :
	usageCount(0),
	glTexId(_glTexId),
	filename("[no file name]"),
	imageWidth(_imsize.width), imageHeight(_imsize.height),
	textureWidth(_texsize.width), textureHeight(_texsize.height)
{
	// Protected constr, used for creating the placeholder texture
}

W::Texture::~Texture()
{
	if (usageCount > 0) {
		std::stringstream ss;
		ss
			<< "Error deallocating texture '" << filename << "': "
			<< usageCount << " DrawnObjs are still using it.";
		throw Exception(ss.str());
	}
	glDeleteTextures(1, &glTexId);
}

void W::Texture::destroy() {
	if (usageCount > 0) {
		std::stringstream ss;
		ss
			<< "Error unloading texture '" << filename << "': "
			<< usageCount << " DrawnObjs are still using it.";
		throw Exception(ss.str());
	}
	_lock_mutex(&texture_mutex);
	_textures_to_unload.push_back(this);
	_unlock_mutex(&texture_mutex);
}

void W::Texture::upload() {
	using std::string;
	glTexId = SOIL_create_OGL_texture(
		p2imagedata,
		textureWidth, textureHeight, NCHANNELS,
		SOIL_CREATE_NEW_ID,
		0
	);
	free(p2imagedata);
	if (!glTexId) {
		string msg =
			string("Could not load texture '") + filename + string("': ") +
			string("error uploading to the GPU");
		throw Exception(msg);
	}
	_uploaded = true;
}

void W::Texture::createPlaceholderTexture() {
	// Create a transparent, 8 x 8 texture to use as a placeholder
	int sz = NCHANNELS * 8 * 8;
	unsigned char *tdata = (unsigned char *) calloc(sz, sizeof(unsigned char));
	if (!tdata)
		throw Exception("Memory allocation error creating default texture");
	
	unsigned int glTexId = SOIL_create_OGL_texture(
		tdata,
		8, 8, NCHANNELS,
		SOIL_CREATE_NEW_ID,
		0
	);
	free(tdata);
	if (!glTexId)
		throw Exception("Could not load placeholder texture: error uploading to the GPU");
	
	_placeholderTexture = new Texture(glTexId, size(8,8), size(8,8));
}
