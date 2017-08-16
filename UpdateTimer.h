/*
 * W - a tiny 2D game development library
 *
 * =================
 *  UpdateTimer.h
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// UpdateTimer is the timer used by W to run the update loop

#ifndef __W__UpdateTimer
#define __W__UpdateTimer

#include "Callback.h"

namespace W {
	
	class UpdateTimer {
		typedef void (*vdfncb)(void);
	public:
		UpdateTimer(vdfncb _fp);
		~UpdateTimer();
		
		void start();
		void stop();
	private:
		void createTimer();
		void destroyTimer();
		
		bool running;
		
		struct Objs;
		Objs *objs;
		
		struct Init;
		static Init *init;
		
		void (*fp)(void);
	};
	
}

#endif
