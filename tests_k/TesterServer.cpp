#include "TesterServer.hpp"

TesterServer::TesterServer()
{
	loc = "/example/sousdoc/////.././.././%4F/%20/%42/example2?test=2";
}

TesterServer::~TesterServer()
{
	;
}

void TesterServer::_assert_same(bool boolean)
{
	if (!boolean)
		std::cout << "	" << "\033[0;32m" << "Match" << "\033[0m" << std::endl;
	else
		std::cout << "	" << "\033[0;31m" << "Not a match" << "\033[0m" << std::endl;
}

void	TesterServer::formatingUri(void)
{
	delete_uri_variable();
	remove_simple_dot();
	format_double_dot();
	decode_uri();
	delete_duplicate_slash();
}

void	TesterServer::add_route(void)
{
	std::cout << "------- add_route" << std::endl;
	Route defaultphp;
	defaultphp.location = "";
	defaultphp.ext = "php";
	Route defaultpy;
	defaultpy.location = "";
	defaultpy.ext = "py";
	Route directory;
	directory.location = "directory/";
	Route directoryDuplica;
	directoryDuplica.location = "directory/";
	Route directoryphp;
	directoryphp.location = "directory/";
	directoryphp.ext = "php";
	Route directorypy;
	directorypy.location = "directory/";
	directorypy.ext = "py";
	Route directorytest;
	directorytest.location = "directory/test";
	try {
		server.add_route(defaultphp);
		_assert_same(0);
		server.add_route(defaultpy);
		_assert_same(0);
		server.add_route(directory);
		_assert_same(0);
		server.add_route(directoryphp);
		_assert_same(0);
		server.add_route(directorypy);
		_assert_same(0);
		server.add_route(directorytest);
		_assert_same(0);
		server.add_route(directoryDuplica);
		_assert_same(1);
	}
	catch (std::exception const & e)
	{
		_assert_same(0);
	}
}

void	TesterServer::start(void)
{
	std::cout << "------- start -------" << std::endl;
	formatingUri();
	add_route();
	choose_route();
}

void	TesterServer::choose_route(void)
{
	Route route;
	std::string expectedloc;
	std::string expectedext;
	std::cout << "------- choose route" << std::endl;
	expectedloc = "directory/";
	expectedext = "php";
	route = server.choose_route("GET /directory/test.php HTTP/1.1");
	_assert_same(route.location.compare(expectedloc) || route.ext.compare(expectedext));
	#ifdef DEBUG
		std::cout << "[" << route.location << "]" << std::endl << "[" << expectedloc << "]" << std::endl;
		std::cout << "[" << route.ext << "]" << std::endl << "[" << expectedext << "]" << std::endl;
	#endif

	expectedext = "py";
	route = server.choose_route("GET /directory/test.py HTTP/1.1");
	_assert_same(route.location.compare(expectedloc) || route.ext.compare(expectedext));
	#ifdef DEBUG
		std::cout << "[" << route.location << "]" << std::endl << "[" << expectedloc << "]" << std::endl;
		std::cout << "[" << route.ext << "]" << std::endl << "[" << expectedext << "]" << std::endl;
	#endif

	expectedext = "";
	route = server.choose_route("GET /directory/test.html HTTP/1.1");
	_assert_same(route.location.compare(expectedloc) || route.ext.compare(expectedext));
	#ifdef DEBUG
		std::cout << "[" << route.location << "]" << std::endl << "[" << expectedloc << "]" << std::endl;
		std::cout << "[" << route.ext << "]" << std::endl << "[" << expectedext << "]" << std::endl;
	#endif

	expectedloc = "";
	expectedext = "php";
	route = server.choose_route("GET /dontexist/test.php HTTP/1.1");
	_assert_same(route.location.compare(expectedloc) || route.ext.compare(expectedext));
	#ifdef DEBUG
		std::cout << "[" << route.location << "]" << std::endl << "[" << expectedloc << "]" << std::endl;
		std::cout << "[" << route.ext << "]" << std::endl << "[" << expectedext << "]" << std::endl;
	#endif
}

void TesterServer::delete_uri_variable(void)
{
	std::string expected = "/example/sousdoc/////.././.././%4F/%20/%42/example2";

	std::cout << "------- delete_uri_variable" << std::endl;
	server._delete_uri_variable(loc);
	_assert_same(loc.compare(expected));

	#ifdef DEBUG
		std::cout << "[" << loc << "]" << std::endl << "[" << expected << "]" << std::endl;
	#endif
}
void TesterServer::remove_simple_dot(void)
{
	std::string expected = "/example/sousdoc/////../../%4F/%20/%42/example2";

	std::cout << "------- remove_simple_dot" << std::endl;
	server._remove_simple_dot(loc);
	_assert_same(loc.compare(expected));

	#ifdef DEBUG
		std::cout << "[" << loc << "]" << std::endl << "[" << expected << "]" << std::endl;
	#endif
}
void TesterServer::format_double_dot(void)
{
	std::string expected = "/example/sousdoc///%4F/%20/%42/example2";

	std::cout << "------- format_double_dot" << std::endl;
	server._format_double_dot(loc);
	_assert_same(loc.compare(expected));

	#ifdef DEBUG
		std::cout << "[" << loc << "]" << std::endl << "[" << expected << "]" << std::endl;
	#endif
}
void TesterServer::decode_uri(void)
{
	std::string expected = "/example/sousdoc///O/ /B/example2";

	std::cout << "------- decode_uri" << std::endl;
	server._decode_uri(loc);
	_assert_same(loc.compare(expected));

	#ifdef DEBUG
		std::cout << "[" << loc << "]" << std::endl << "[" << expected << "]" << std::endl;
	#endif
}
void TesterServer::delete_duplicate_slash(void)
{
	std::string expected = "/example/sousdoc/O/ /B/example2";

	std::cout << "------- delete_duplicate_slash" << std::endl;
	server._delete_duplicate_slash(loc);
	_assert_same(loc.compare(expected));

	#ifdef DEBUG
		std::cout << "[" << loc << "]" << std::endl << "[" << expected << "]" << std::endl;
	#endif
}