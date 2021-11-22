#include "../inc/Route.hpp"

Route::Route()
{
	root.first = "/www";
	root.second = "false";
	autoindex = "off";
	max_body_size = "100 000 000";
	location = "/";
	limit_except = "";
	error_page; // a definir
	return_ = "";
	ext = "";
	index.push_back("index.html");
	index.push_back("index.php");
	index.push_back("index.py");
	cgi = "";
	upload = "/uploads";
}

Route::~Route()
{
	;
}

void Route::what()
{
	std::cout << "------ Route ------" << std::endl;
	std::cout << "root : " << root.first << " " << root.second << std::endl;
	std::cout << "autoindex : " << autoindex << std::endl;
	std::cout << "max_body_size : " << max_body_size << std::endl;
	std::cout << "location : " << location << std::endl;
	std::cout << "limit_except : " << limit_except << std::endl;

	std::map<std::string, std::string>::iterator iterr = error_page.begin();
	std::map<std::string, std::string>::iterator iterre = error_page.end();
	std::cout << "error_page : " << std::endl;
	while (iterr != iterre)
	{
		std::cout << (*iterr).first << " " << (*iterr).second << std::endl;
		iterr++;
	}
	
	std::cout << "return : " << return_ << std::endl;
	std::cout << "ext : " << ext << std::endl;

	std::vector<std::string>::iterator it = index.begin();
	std::vector<std::string>::iterator ite = index.end();
	std::cout << "index :" << std::endl;
	while (it != ite)
	{
		std::cout << *it << std::endl;
		it++;
	}
	std::cout << "cgi : " << cgi << std::endl;
	std::cout << "upload : " << upload << std::endl;
	;
}
