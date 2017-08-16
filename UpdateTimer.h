/*
 * UpdateTimer.h - timer to run the update loop
 *
 */

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
