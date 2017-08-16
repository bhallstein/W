#ifndef __W__Callback
#define __W__Callback

namespace W {
	
	class Event;
	
	class CallbackBase {
	public:
		virtual ~CallbackBase() { }
		virtual void call(Event *) = 0;
		virtual CallbackBase* clone() = 0;
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
		CallbackBase* clone() {
			return new MFCallback<T>(f, o);
		}
	private:
		mftype f;
		T *o;
	};
	
	class Callback {
	public:
		template <class T>
		Callback(void (T::*_f)(Event *), T *_o) : s(new MFCallback<T>(_f, _o)), resp(_o) { }
		Callback(CallbackBase *_s, EventResponder *_resp) : s(_s), resp(_resp) { }
		~Callback() { delete s; }
		void call(Event *ev) {
			s->call(ev);
		}
		Callback* copy() const {
			return new Callback(s->clone(), resp);
		}
		EventResponder *resp;
	private:
		CallbackBase *s;
	};
	
}

#endif
