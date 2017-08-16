#ifndef __W__Callback
#define __W__Callback

#include <iostream>

namespace W {
	
	class Event;
	
	class CallbackBase {
	public:
		virtual ~CallbackBase() { }
		virtual void call(Event *) = 0;
		virtual CallbackBase* copy() = 0;
	};
	
	template <class T>
	class MFCallback : public CallbackBase {
		typedef void (T::*mftype)(Event *);
	public:
		MFCallback(mftype _f, T *_o) : f(_f), o(_o) { }
		~MFCallback() { }
		void call(Event *ev) {
			(o->*f)(ev);
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
		Callback(void (T::*_f)(Event *), T *_o) :
			c(new MFCallback<T>(_f, _o)), resp(_o)
		{
			// constr
		}
		~Callback()
		{
			delete c;
		}
		void call(Event *ev) {
			c->call(ev);
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
