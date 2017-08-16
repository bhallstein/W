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
		virtual void call() { }
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
	
	
	template <class T>
	class VoidCallback : public CallbackBase {
		typedef void (T::*vftype)(void);
	public:
		VoidCallback(vftype _f, T *_o) : f(_f), o(_o) { }
		~VoidCallback() { }
		void call() {
			(o->*f)();
		}
		CallbackBase* copy() {
			return new VoidCallback<T>(f, o);
		}
	protected:
		vftype f;
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
		template <class T>
		Callback(void (T::*_f)(void), T *_o) :
			c(new VoidCallback<T>(_f, _o)), resp(_o)
		{
			// constr for void member function callbacks
		}
		~Callback()
		{
			delete c;
		}
		void call() {
			return c->call();
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
