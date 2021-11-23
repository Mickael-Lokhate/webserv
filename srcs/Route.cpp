#include "../inc/Route.hpp"

Route::Route()
{
	root.first = "/www";
	root.second = false;
	autoindex = "off";
	max_body_size = "1 000 000";
	location = "www";
	limit_except = "";
	init_error_page();
	return_ = "";
	ext = "";
	index.push_back("index.html");
	cgi = "";
	upload = "uploads";
}

Route::~Route()
{
	;
}

void Route::init_error_page(void)
{
	error_page.insert(std::pair<std::string, std::string>("403", "error/403.html"));
	error_page.insert(std::pair<std::string, std::string>("404", "error/404.html"));
	error_page.insert(std::pair<std::string, std::string>("500", "error/50x.html"));
	error_page.insert(std::pair<std::string, std::string>("501", "error/501.html"));
	error_page.insert(std::pair<std::string, std::string>("502", "error/50x.html"));
	error_page.insert(std::pair<std::string, std::string>("503", "error/50x.html"));
	error_page.insert(std::pair<std::string, std::string>("504", "error/50x.html"));
}

void Route::what()
{
	std::cout << "------ Route : " << location << " ------" << std::endl;
	if (!root.second)
		std::cout << "root : " << root.first << std::endl;
	else
		std::cout << "alias : " << root.first << std::endl;
	std::cout << "autoindex : " << autoindex << std::endl;
	std::cout << "max_body_size : " << max_body_size << std::endl;
	std::cout << "limit_except : " << limit_except << std::endl;

	std::map<std::string, std::string>::iterator iterr = error_page.begin();
	std::map<std::string, std::string>::iterator iterre = error_page.end();
	std::cout << "error_page : " << std::endl;
	while (iterr != iterre)
	{
		std::cout << "	" << (*iterr).first << " " << (*iterr).second << std::endl;
		iterr++;
	}
	
	std::cout << "return : " << return_ << std::endl;
	std::cout << "ext : " << ext << std::endl;

	std::vector<std::string>::iterator it = index.begin();
	std::vector<std::string>::iterator ite = index.end();
	std::cout << "index :" << std::endl;
	while (it != ite)
	{
		std::cout << "	" << *it << std::endl;
		it++;
	}
	std::cout << "cgi : " << cgi << std::endl;
	std::cout << "upload : " << upload << std::endl;
	;
}
