/*
 * Controller.h - static object which runs W
 *
 */

#ifndef __W__Controller
#define __W__Controller

#include "types.h"

namespace W {
	
	class Window;
	class UpdateTimer;
	class Timer;
	
	class Controller {
	public:
		Controller();
		~Controller();
		void createWindow(const W::size &sz, const std::string &title);
		void start();
		
		void update();
		void updateAllViewPositions();
		Window *window;
		
	private:
		UpdateTimer *updateTimer;
		
		bool firstUpdate;
		
		void quit();
		
		Timer *timer;
	};
	
	
	extern Controller _controller;
	#ifdef WTARGET_IOS
		void createWindow();
	#else
		void createWindow(const W::size &sz, const std::string &title);
	#endif
	void start();
	
	extern int updateMicroseconds;
	
}

#endif
