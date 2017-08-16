#ifndef __W__Callback
#define __W__Callback

#include <iostream>

namespace W {
	
	class Event;
	
	namespace EventPropagation {
		enum T {
			SHOULD_STOP, SHOULD_CONTINUE
		};
	}
	
	class CallbackBase {
	public:
		virtual ~CallbackBase() { }
		virtual EventPropagation::T call(Event *) = 0;
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
			// constr
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
