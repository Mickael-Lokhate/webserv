#include "Route.hpp"

Route::Route()
{
	root.first = "/www";
	root.second = false;
	autoindex = "off";
	max_body_size = "1000000";
	location = "";
	init_error_page();
	ext = "";
	index.push_back("index.html");
	index.push_back("index.php");
	cgi = "";
	upload = "";
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

void Route::what() const
{
	std::cout << "------ Route : " << location << " ------" << std::endl;
	if (!root.second)
		std::cout << "root : " << root.first << std::endl;
	else
		std::cout << "alias : " << root.first << std::endl;
	std::cout << "autoindex : " << autoindex << std::endl;
	std::cout << "max_body_size : " << max_body_size << std::endl;

	std::vector<std::string>::const_iterator itexc = limit_except.begin();
	std::vector<std::string>::const_iterator itexce = limit_except.end();
	std::cout << "limit_except : " << std::endl;
	while (itexc != itexce)
	{
		std::cout << "	" << *itexc << std::endl;
		itexc++;
	}

	std::map<std::string, std::string>::const_iterator iterr = error_page.begin();
	std::map<std::string, std::string>::const_iterator iterre = error_page.end();
	std::cout << "error_page : " << std::endl;
	while (iterr != iterre)
	{
		std::cout << "	" << (*iterr).first << " " << (*iterr).second << std::endl;
		iterr++;
	}

	std::cout << "return : " << std::endl;
	std::cout << "	" << return_.first << " " << return_.second << std::endl;

	std::cout << "ext : " << ext << std::endl;

	std::vector<std::string>::const_iterator it = index.begin();
	std::vector<std::string>::const_iterator ite = index.end();
	std::cout << "index :" << std::endl;
	while (it != ite)
	{
		std::cout << "	" << *it << std::endl;
		it++;
	}
	std::cout << "cgi : " << cgi << std::endl;
	std::cout << "upload : " << upload << std::endl;
}
