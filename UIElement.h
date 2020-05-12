/*
 * W - a tiny 2D game development library
 *
 * ===============
 *  UIElement.h
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__UIElement
#define __W__UIElement

#include "types.h"
#include "Event.h"
#include "Positioner.h"
#include "Callback.h"

#include <string>
#include <map>

namespace W {

	class View;
	class Rectangle;
  class RetroText;

	class UIElement {
	public:
		UIElement(const std::string &_name, W::Positioner, View *);
		~UIElement();
		
		void _updatePosition(v2i containersize);
		virtual void updatePosition() { }
			// Override to implement updates to D.O.s etc
		
		virtual void activate() = 0;
		virtual void deactivate() = 0;
		
	protected:
		std::string name;
		W::Positioner positioner;
		fRect rct;
		View *view;
	};
	
	
	class Button : public UIElement {
	public:
		Button(std::string _name, W::Positioner, View *);
		~Button();
		EventPropagation::T recEv(Event);
		virtual void activate();
    virtual void deactivate();
    virtual void updatePosition();
	protected:
		bool hover;
		bool active;
		Event buttonClickEvent;
		Rectangle *btnrect;
	};


  class TextButton : public Button {
  public:
    TextButton(std::string text, std::string name, W::Positioner, View*);
    TextButton(std::string text, std::string name, View*);
    ~TextButton();
    virtual void activate();
    virtual void deactivate();
    virtual void updatePosition();
  protected:
    std::string str;
    RetroText *btntext;
  };

}

#endif
