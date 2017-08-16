/*
 * oglDrawHelpers.h
 *
 */

#ifndef __W_oglDrawHelpers_h
#define __W_oglDrawHelpers_h

#include "W.h"
#ifdef __APPLE__
	#include <OpenGL/gl.h>
#elif defined _WIN32 || _WIN64
	#include <gl\gl.h>
	#include <gl\glu.h>
#endif

namespace W {
	
	namespace GLState {
		bool texturing_enabled = false;
	}
	
	inline void drawRect(float x, float y, float w, float h, const W::Colour &col, float rot = 0) {
		if (GLState::texturing_enabled) {
			glDisable(GL_TEXTURE_2D);
			GLState::texturing_enabled = false;
		}
		glColor4f(col.r, col.g, col.b, col.a);
		
		glLoadIdentity();
		glTranslatef(x + w/2, y + h/2, 0);
		glRotatef(rot, 0, 0, 1);
		glBegin(GL_QUADS);
		{
			glVertex2f(-w/2, -h/2);
			glVertex2f(w/2, -h/2);
			glVertex2f(w/2, h/2);
			glVertex2f(-w/2, h/2);
		}
		glEnd();
	}
	
	inline void drawImage(float x, float y, float w, float h, float texA, float texB, float texC, float texD, GLuint texId, float opac = 1, float rot = 0) {
		if (!GLState::texturing_enabled) {
			glEnable(GL_TEXTURE_2D);
			GLState::texturing_enabled = true;
		}
		glColor4f(1, 1, 1, opac);
		glLoadIdentity();
		glTranslatef(x + w/2, y + h/2, 0);
		glRotatef(rot, 0, 0, 1);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(texA, texB); glVertex2f(-w/2, -h/2);
			glTexCoord2f(texC, texB); glVertex2f(w/2, -h/2);
			glTexCoord2f(texC, texD); glVertex2f(w/2, h/2);
			glTexCoord2f(texA, texD); glVertex2f(-w/2, h/2);
		}
		glEnd();
	}
	
	inline void drawText(float x, float y, W::Colour &col, const char *text, bool rAlign = false) {
		int char_width = 14;
		// work out total width & hence position
		int tw = 0, c;
		for (int i=0; (c = text[i]); i++)
			if (c == 'I' | c == 'i' || c == '1' || c == ':' || c == '!' || c == '.' || c == '\'') tw += char_width - 4;
			else if (c == 'L' || c == 'l') tw += char_width - 2;
			else tw += char_width;
		if (rAlign) x -= tw;
		
		for (int i=0; text[i] != '\0'; i++) {
			char c = text[i];
			switch(c) {
				case 'A' : case 'a' : {
					drawRect(x, y, 10, 2, col, 0);
					drawRect(x, y+2, 2, 8, col, 0);
					drawRect(x+2, y+4, 6, 2, col, 0);
					drawRect(x+8, y+2, 2, 8, col, 0);
					break;
				}
				case 'B' : case 'b' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y, 6, 2, col, 0);
					drawRect(x+2, y+4, 6, 2, col, 0);
					drawRect(x+2, y+8, 6, 2, col, 0);
					drawRect(x+8, y+2, 2, 8, col, 0);
					break;
				}
				case 'C' : case 'c' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y, 7, 2, col, 0);
					drawRect(x+2, y+8, 7, 2, col, 0);
					break;
				}
				case 'D': case 'd' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+8, y+2, 2, 8, col, 0);
					drawRect(x+2, y, 6, 2, col, 0);
					drawRect(x+2, y+8, 6, 2, col, 0);
					break;
				}
				case 'E' : case 'e' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y, 7, 2, col, 0);
					drawRect(x+2, y+4, 5, 2, col, 0);
					drawRect(x+2, y+8, 7, 2, col, 0);
					break;
				}
				case 'F' : case 'f' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y, 7, 2, col, 0);
					drawRect(x+2, y+4, 5, 2, col, 0);
					break;
				}
				case 'G' : case 'g' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y, 8, 2, col, 0);
					drawRect(x+2, y+8, 8, 2, col, 0);
					drawRect(x+8, y+4, 2, 4, col, 0);
					break;
				}
				case 'H' : case 'h' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x+2, y+4, 6, 2, col, 0);
					break;
				}
				case '1' : {
					drawRect(x, y, 2, 2, col, 0);
				}
				case 'I' : case 'i' : {
					drawRect(x+2, y, 2, 10, col, 0);
					break;
				}
				case 'J' : case 'j' : {
					drawRect(x+6, y, 2, 10, col, 0);
					drawRect(x, y+8, 6, 2, col, 0);
					break;
				}
				case 'K' : case 'k' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y+4, 5, 2, col, 0);
					drawRect(x+7, y, 2, 4, col, 0);
					drawRect(x+7, y+6, 2, 4, col, 0);
					break;
				}
				case 'L' : case 'l' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y+8, 6, 2, col, 0);
					break;
				}
				case 'M' : case 'm' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y+2, 2, 2, col, 0);
					drawRect(x+4, y+4, 2, 2, col, 0);
					drawRect(x+6, y+2, 2, 2, col, 0);
					drawRect(x+8, y, 2, 10, col, 0);
					break;
				}
				case 'N' : case 'n' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y+2, 2, 2, col, 0);
					drawRect(x+4, y+4, 2, 2, col, 0);
					drawRect(x+6, y+6, 2, 2, col, 0);
					drawRect(x+8, y, 2, 10, col, 0);
					break;
				}
				case '0' : {
					drawRect(x+4, y+4, 2, 2, col, 0);
				}
				case 'O' : case 'o' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x+2, y, 6, 2, col, 0);
					drawRect(x+2, y+8, 6, 2, col, 0);
					break;
				}
				case 'P' : case 'p' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y, 6, 2, col, 0);
					drawRect(x+2, y+4, 6, 2, col, 0);
					drawRect(x+8, y, 2, 6, col, 0);
					break;
				}
				case 'Q' : case 'q' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x+2, y, 6, 2, col, 0);
					drawRect(x+2, y+8, 6, 2, col, 0);
					drawRect(x+6, y+10, 2, 2, col, 0);
					break;
				}
				case 'R' : case 'r' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y, 6, 2, col, 0);
					drawRect(x+2, y+4, 6, 2, col, 0);
					drawRect(x+8, y, 2, 6, col, 0);
					drawRect(x+6, y+6, 2, 2, col, 0);
					drawRect(x+8, y+8, 2, 2, col, 0);
					break;
				}
				case 'S' : case 's' : case '5' : {
					drawRect(x, y, 10, 2, col, 0);
					drawRect(x, y+4, 10, 2, col, 0);
					drawRect(x, y+8, 10, 2, col, 0);
					drawRect(x, y+2, 2, 2, col, 0);
					drawRect(x+8, y+6, 2, 2, col, 0);
					break;
				}
				case 'T' : case 't' : {
					drawRect(x, y, 10, 2, col, 0);
					drawRect(x+4, y+2, 2, 8, col, 0);
					break;
				}
				case 'U' : case 'u' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x+2, y+8, 6, 2, col, 0);
					break;
				}
				case 'V' : case 'v' : {
					drawRect(x, y, 2, 6, col, 0);
					drawRect(x+2, y+6, 2, 2, col, 0);
					drawRect(x+4, y+8, 2, 2, col, 0);
					drawRect(x+6, y+6, 2, 2, col, 0);
					drawRect(x+8, y, 2, 6, col, 0);
					break;
				}
				case 'W' : case 'w' : {
					drawRect(x, y, 2, 8, col, 0);
					drawRect(x+4, y, 2, 8, col, 0);
					drawRect(x+8, y, 2, 8, col, 0);
					drawRect(x+2, y+8, 2, 2, col, 0);
					drawRect(x+6, y+8, 2, 2, col, 0);
					break;
				}
				case 'X' : case 'x' : {
					drawRect(x, y, 2, 2, col, 0);
					drawRect(x+2, y+2, 2, 2, col, 0);
					drawRect(x+4, y+4, 2, 2, col, 0);
					drawRect(x+6, y+6, 2, 2, col, 0);
					drawRect(x+8, y+8, 2, 2, col, 0);
					drawRect(x+8, y, 2, 2, col, 0);
					drawRect(x+6, y+2, 2, 2, col, 0);
					drawRect(x+2, y+6, 2, 2, col, 0);
					drawRect(x, y+8, 2, 2, col, 0);
					break;
				}
				case 'Y' : case 'y' : {
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x, y, 2, 2, col, 0);
					drawRect(x+2, y+2, 2, 2, col, 0);
					drawRect(x+4, y+4, 4, 2, col, 0);
					break;
				}
				case 'Z' : case 'z' : {
					drawRect(x, y, 10, 2, col, 0);
					drawRect(x, y+8, 10, 2, col, 0);
					drawRect(x+6, y+2, 2, 2, col, 0);
					drawRect(x+4, y+4, 2, 2, col, 0);
					drawRect(x+2, y+6, 2, 2, col, 0);
					break;
				}
				case '2' : {
					drawRect(x, y, 2, 4, col, 0);
					drawRect(x+2, y, 8, 2, col, 0);
					drawRect(x+8, y+2, 2, 2, col, 0);
					drawRect(x+2, y+4, 8, 2, col, 0);
					drawRect(x, y+6, 2, 4, col, 0);
					drawRect(x+2, y+8, 8, 2, col, 0);
					break;
				}
				case '3' : {
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x, y, 8, 2, col, 0);
					drawRect(x, y+8, 8, 2, col, 0);
					drawRect(x+2, y+4, 6, 2, col, 0);
					break;
				}
				case '4' : {
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x, y, 2, 4, col, 0);
					drawRect(x, y+4, 8, 2, col, 0);
					break;
				}
				case '6' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+2, y, 8, 2, col, 0);
					drawRect(x+2, y+4, 8, 2, col, 0);
					drawRect(x+2, y+8, 8, 2, col, 0);
					drawRect(x+8, y+6, 2, 2, col, 0);
					break;
				}
				case '7' : {
					drawRect(x, y, 10, 2, col, 0);
					drawRect(x+8, y+2, 2, 2, col, 0);
					drawRect(x+6, y+4, 2, 2, col, 0);
					drawRect(x+4, y+6, 2, 2, col, 0);
					drawRect(x+2, y+8, 2, 2, col, 0);
					break;
				}
				case '8' : {
					drawRect(x, y, 2, 10, col, 0);
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x+2, y, 6, 2, col, 0);
					drawRect(x+2, y+4, 6, 2, col, 0);
					drawRect(x+2, y+8, 6, 2, col, 0);
					break;
				}
				case '9' : {
					drawRect(x+8, y, 2, 10, col, 0);
					drawRect(x, y, 8, 2, col, 0);
					drawRect(x, y+4, 8, 2, col, 0);
					drawRect(x, y+2, 2, 2, col, 0);
					drawRect(x, y+8, 8, 2, col, 0);
					break;
				}
					//			case '$' : {
					//				drawRect(x, y, 10, 2, col, 0);
					//				drawRect(x, y+4, 10, 2, col, 0);
					//				drawRect(x, y+8, 10, 2, col, 0);
					//				drawRect(x, y+2, 2, 2, col, 0);
					//				drawRect(x+8, y+6, 2, 2, col, 0);
					//				drawRect(x+4, y-1, 2, 12, col, 0);
					//				break;
					//			}
				case MR_CURRENCY /* £ */ : {
					drawRect(x+4, y, 6, 2, col, 0);
					drawRect(x+2, y+2, 2, 2, col, 0);
					drawRect(x, y+4, 8, 2, col, 0);
					drawRect(x+2, y+6, 2, 4, col, 0);
					drawRect(x, y+8, 10, 2, col, 0);
					break;
				}
				case ':' : {
					drawRect(x+2, y+4, 2, 2, col, 0);
					drawRect(x+2, y+8, 2, 2, col, 0);
					break;
				}
				case '!' : {
					drawRect(x+2, y, 2, 6, col, 0);
					drawRect(x+2, y+8, 2, 2, col, 0);
					break;
				}
				case '.' : {
					drawRect(x+2, y+8, 2, 2, col, 0);
					break;
				}
				case '\'' : {
					drawRect(x+2, y, 2, 2, col, 0);
					drawRect(x+3, y-2, 2, 2, col, 0);
					break;
				}
				default: break;
			}
			if (c == 'I' | c == 'i' || c == '1' || c == ':' || c == '!' || c == '.' || c == '\'') x += char_width - 4;
			else if (c == 'L' || c == 'l')                   x += char_width - 2;
			else                                             x += char_width;
		}
	}
	
}

#endif
