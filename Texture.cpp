#include "types.h"
#include "W.h"
#include "MegaTexture.h"

W::Texture *W::Texture::_whiteTexture = NULL;

W::Texture::Texture(const std::string &_filename) :
	mtNode(NULL),
	usageCount(0),
	filename(_filename)
{
	if (!MegaTexture::addTex(_filename, this))
		throw Exception("Error creating texture, see log for details");
}

W::Texture::~Texture()
{
	if (usageCount > 0) {
		std::stringstream ss;
		ss << "Error deallocating texture '" << filename << "': " << usageCount << " DObjs are still using it.";
		throw Exception(ss.str());
	}
	if (mtNode) MegaTexture::removeTex(mtNode);
}

float W::Texture::floatCoordA(int texA) { return float(mtA + texA + 0.5) / MegaTexture::width(); }
float W::Texture::floatCoordB(int texB) { return float(mtB + texB + 0.5) / MegaTexture::width(); }
float W::Texture::floatCoordC(int texC) { return float(mtA + texC - 0.5) / MegaTexture::width(); }
float W::Texture::floatCoordD(int texD) { return float(mtB + texD - 0.5) / MegaTexture::width(); }
	// Note: texels are sampled in the middle. We add/subtract half a pixel to prevent unwanted edge blending.
	// See also http://www.mindcontrol.org/~hplus/graphics/opengl-pixel-perfect.html

void W::Texture::_createWhiteTexture() {
	if (_whiteTexture) return;
	_whiteTexture = new Texture();
	unsigned char *data = (unsigned char *) malloc(sizeof(unsigned char) * 32 * 32 * N_CHANNELS);
	for (int i=0; i < 32*32*N_CHANNELS; ++i)
		data[i] = 255;
	MegaTexture::addTex(data, size(32,32), _whiteTexture);
	free(data);
}
