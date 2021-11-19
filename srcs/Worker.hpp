#ifndef WORKER_HPP
#define WORKER_HPP
/*
		+---------------------------------------------------------------------+
		|                                                                     |
		|                              Worker                                 |
		|                                                                     |
		|                                                                     |
		|                           event_loop()                              |
		|                                                                     |
		|     +---------------------------+                                   |
		|     |                                                               |
		|     +--> new_client()+--------------------------------> accept()    |
		|     |                                                               |
		|     +--> recv_client()+-+                                           |
		|     |                   |                                           |
		|     +--> send_client()+------------------------+                    |
		|     |                   |                      |                    |
		|     +--> del_client()+--------------------------------> close()     |
		|                         |                      |                    |
		|                         |                      |                    |
		|                         |                      |                    |
		|                         v                      v                    |
		|                  build_request()       build_response()             |
		|                         +                      +                    |
		|                         |                      |                    |
		|    +--------------------+                      |                    |
		|    |                                           |                    |
		|    |                                           |                    |
		|  +---------------------------------------------+                    |
		|  | |          +---------------------------------------------------+ |
		|  | +----> req |GET / HTTP/1.1\r\nHost: example.com\r\nGET / HTTP/1| |
		|  |            +---------------------------------------------------+ |
		|  |                                                                  |
		|  |  * Aggrégation des données client                      ^         |
		|  |                                                        |         |
		|  |  * Vérification de la requête                          +         |
		|  |                                                      cursor      |
		|  |  * Création de l'object Request                                  |
		|  |                                                                  |
		|  |               ----------+ Gestion Client +----------             |
		|  |                                                                  |
		|  |  * Construction de la réponse(fichiers, CGI...)                  |
		|  |                                                                  |
		|  |  * Récupération des fichiers associés                            |
		|  |                                                                  |
		|  |  * Emission des données, fragmentée si nécessaire                |
		|  |                                                                  |
		|  |                                                                  |
		|  |            +---------------------------------------------------+ |
		|  +------> rep |HTTP 200 OK\r\nContent_type: blablabla...          | |
		|               +---------------------------------------------------+ |
		+---------------------------------------------------------------------++
*/
class Worker {

	public:

	class Client {

		public :

		/* parser */
		void build_request(const std::string &);
		/* builder */
		void build_response(void);

		private :

		std::string req;
		std::string rep;
	};

	Worker(const Server &, const Socket &);
	void event_loop(void);
	void to_string(void) const;

	private:

	void new_client(void);
	void recv_client(const Client &);
	void send_client(const Client &);
	void del_client(const Client &);

	map<int, Client>			c;
	const vector<Server> & 		r;
	const map<Socket, Server> &	s;
};
#endif
