#include "Master.hpp"

int main(int ac, char ** argv[]) {
	Matser master(argv[1]);
	master.init();
	master.work();
}
