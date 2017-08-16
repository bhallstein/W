/*
 * Texture.h - represents an OpenGL texture
 *
 */

#ifndef __W__Texture
#define __W__Texture

#include <string>

namespace W {
	
	class Texture {
	public:
		Texture(const std::string &_filename);
		~Texture();
			// Must be called from the thread on which the OpenGL context is current
		
		void destroy();
			// User should not call a texture’s destructor, but call destroy().
			// The texture will then be deallocated from the appropriate thread.
			// Before destroying the texture, all DrawnImgs using the texture
			// should be deallocated.
		
		void incrementUsageCount() { ++usageCount; }
		void decrementUsageCount() { --usageCount; }
		
		inline unsigned int getTexId() { return glTexId; }
		inline float getFloatW() { return float(imageWidth) / float(textureWidth); }
		inline float getFloatH() { return float(imageHeight) / float(textureHeight); }
		
		void upload();
			// Must be called from thread for which the OpenGL
			// context is current.
		bool _uploaded;
		
		static void createPlaceholderTexture();
		static Texture *_placeholderTexture;
			// A texture used as placeholder for textures that have not yet been
			// uploaded on the appropriate thread.
		
	protected:
		Texture(unsigned int _glTexId, W::size _imsize, W::size _texsize);
		unsigned int glTexId;
		int imageWidth, imageHeight;
		int textureWidth, textureHeight;
			// If the image’s dimensions aren’t powers of two, the
			// texture will be larger than the loaded image.
		unsigned char *p2imagedata;
		
		std::string filename;
		
		int usageCount;	// Number of DrawnObjs referencing the texture
	};
	
}

#endif
