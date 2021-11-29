#include <iostream>

void f(std::string & rstr) {
	rstr.clear();
	rstr = "def";
	rstr.resize(10000);
}

int main() {
	std::string str;
	str = "abc";
	printf("string %s, c_str %p \n", str.c_str(), str.c_str());
	f(str);
	printf("string %s, c_str %p \n", str.c_str(), str.c_str());
}
