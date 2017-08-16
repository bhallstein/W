#include "types.h"

bool W::isNum(const char c) {
	return (c >= '0' && c <= '9');
}

void W::strSplit(const std::string &s, std::vector<std::string> &v, char delim) {
	v.clear();
	std::stringstream ss(s);
	std::string chunk;
	while(std::getline(ss, chunk, delim))
		v.push_back(chunk);
}
