/*
 * UpdateTimer.h - timer to run the update loop
 *
 */

#ifndef __W__UpdateTimer
#define __W__UpdateTimer

#include "Callback.h"

namespace W {
	
	class UpdateTimer {
	public:
		UpdateTimer(Callback *);
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
		
		W::Callback *c;
	};
	
}

#endif
