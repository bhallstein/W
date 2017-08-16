#include "Log.h"
#include "types.h"

struct W::_WLogInit {
	_WLogInit() {
		// Default log: /dev/null
		std::string p = "/dev/null";
		#if defined WTARGET_WIN
			p = "\\Device\\Null";
		#endif
		setLogPath(p.c_str());
	}
};

std::ofstream W::log;
W::_WLogInit *W::_logInit = new W::_WLogInit();

void W::setLogPath(const std::string &path) {
	if (log.is_open())
		log.close();
	log.open(path.c_str());
}
