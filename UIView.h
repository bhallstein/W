#ifndef __W__UIView
#define __W__UIView

#include "View.h"
#include "Event.h"
#include "types.h"

namespace W {
	
	class EventHandler;
	class Callback;
	class Button;
	
	class Button : public MappedObj {
	public:
		Button(const W::rect &, const char *_action);
		~Button();
		
		// Methods
		void setHover();
		W::Colour& col();
		Event* getEvent();
		
	protected:
		// Properties
		bool hover;
		Event event;
		Colour _col;
		Colour _hovcol;
	};
	

	enum Dragability { ALLOW_DRAG = 1, DISALLOW_DRAG = 0 };
	
	class UIView : public View {
	public:
		UIView(Positioner *, Window *, EventHandler *, Dragability _allowDrag = DISALLOW_DRAG);
		~UIView();
		
		void processMouseEvent(Event *);
		void subscribeToButtons(Callback *);
		virtual void draw() { }
		
	protected:
		EventHandler *eh;
		bool allowDrag;
		bool dragloop;
		position drag_initial;
		
		std::vector<Button*> buttons;
		std::vector<Callback*> subscribers;
	};

}

#endif
