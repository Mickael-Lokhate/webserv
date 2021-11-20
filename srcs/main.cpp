#include "Master.hpp"

int main(int ac, char **argv) 
{
	(void)ac;
	Master master(argv[1]);
	master.init();
	//master.work();
}
