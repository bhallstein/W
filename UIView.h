#ifndef __W__UIView
#define __W__UIView

#include "View.h"
#include "UIElement.h"
#include "Event.h"
#include "types.h"

namespace W {
	
	class EventHandler;
	class Callback;
	

	enum Draggability { DISALLOW_DRAG = 0, ALLOW_DRAG };
	
	class UIView : public View {
	public:
		UIView(Positioner *, Window *, Draggability _allowDrag = DISALLOW_DRAG);
		~UIView();
		
		void processMouseEvent(Event *);
		virtual void draw();
		
	protected:
		bool allowDrag;
		bool dragloop;
		position drag_initial;
		
		std::vector<Button*> buttons;

		void createEvTypeMap();			// Translation table for event types subscribed
		UIElement::evTypeMap evTypeMap;	// to by UIElements. e.g. MOUSEMOVE => X
										// (Specific to the UIView, since buttons will subscribe
										// to view-specific mousey events submitted in processMouseEvent)
		
	};

}

#endif
