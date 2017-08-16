#include "types.h"
#include <sstream>

/* Exception */

W::Exception::Exception(const char *_msg, int _err)
{
	std::stringstream ss;
	ss << _msg << " (error: " << _err << ")";
	msg = ss.str();
}
W::Exception::Exception(const std::string &_msg, int _err)
{
	std::stringstream ss;
	ss << _msg << " (error: " << _err << ")";
	msg = ss.str();
}


/* Stringy functions */

bool W::isNum(const char c) {
	return (c >= '0' && c <= '9');
}

void W::upCase(std::string &s) {
	for (std::string::iterator it = s.begin(); it < s.end(); it++) {
		char &c = *it;
		if (c >= 'a' && c <= 'z')
			c += 'A' - 'a';
	}
}
void W::downCase(std::string &s) {
	for (std::string::iterator it = s.begin(); it < s.end(); it++) {
		char &c = *it;
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
	}
}

void W::strSplit(const std::string &s, std::vector<std::string> &v, char delim) {
	v.clear();
	std::stringstream ss(s);
	std::string chunk;
	while(std::getline(ss, chunk, delim))
		v.push_back(chunk);
}

void W::implode(const std::vector<std::string> &v, std::string &s, const char *glue) {
	int i=0;
	std::stringstream ss;
	for (std::vector<std::string>::const_iterator it = v.begin(); it < v.end(); it++, i++) {
		if (i > 0) ss << glue;
		ss << *it;
	}
	s = ss.str();
}
