/*
 * W - a tiny 2D game development library
 *
 * ==============
 *  Callback.h
 * ==============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__Callback
#define __W__Callback

#include <iostream>

namespace W {
	
	class Event;
	
	namespace EventPropagation {
		enum T {
			ShouldStop, ShouldContinue
		};
	}
	
	class CallbackBase {
	public:
		virtual ~CallbackBase() { }
		virtual EventPropagation::T call(Event *) { return W::EventPropagation::ShouldContinue; }
		virtual CallbackBase* copy() = 0;
	};
	
	
	template <class T>
	class MFCallback : public CallbackBase {
		typedef EventPropagation::T (T::*mftype)(Event *);
	public:
		MFCallback(mftype _f, T *_o) : f(_f), o(_o) { }
		~MFCallback() { }
		EventPropagation::T call(Event *ev) {
			return (o->*f)(ev);
		}
		CallbackBase* copy() {
			return new MFCallback<T>(f, o);
		}
	protected:
		mftype f;
		T *o;
	};
	
	
	class Callback {
	public:
		template <class T>
		Callback(EventPropagation::T (T::*_f)(Event *), T *_o) :
			c(new MFCallback<T>(_f, _o)), resp(_o)
		{
			// constr for event response callbacks
		}
		~Callback()
		{
			delete c;
		}
		EventPropagation::T call(Event *ev) {
			return c->call(ev);
		}
		Callback* copy() const {
			return new Callback(c->copy(), resp);
		}
		void *resp;
		
	private:
		Callback(CallbackBase *_c, void *_resp) :
			c(_c), resp(_resp)
		{
			// constr used by copy()
		}
		
		CallbackBase *c;
	};
	
}

#endif
