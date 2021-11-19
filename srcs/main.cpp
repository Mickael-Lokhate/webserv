#include "Master.hpp"

int main(int ac, char **argv) 
{
	Master master(argv[1]);
	master.init();
	master.work();
}
