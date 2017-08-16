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
	
	class Controller {
	public:
		Controller();
		~Controller();
		void createWindow(const W::size &sz, const std::string &title);
		void start();
		
		void updateAllViewPositions();
		
		Window *window;
		
	private:
		UpdateTimer *updateTimer;
		
		void update();
		bool firstUpdate;
		
		void quit();
	};
	
	
	extern Controller _controller;
	void createWindow(const W::size &sz, const std::string &title);
	void start();
	
}

#endif
