/*
 * W - a tiny 2D game development library
 *
 * ================
 *  DebugMacro.h
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// Debug output
// - include DebugMacro.h to enable the w_dout macro
// - to turn output on, define __W_DEBUG before including the file

#ifndef W_DebugMacro_h
#define W_DebugMacro_h

#ifdef __W_DEBUG
	#define w_dout std::cout
#else
	#define w_dout 0 && std::cout
#endif

#endif
