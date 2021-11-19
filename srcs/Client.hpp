struct Client {

	Client(const std::string &, const std::string &);
	Client(const Client &);
	~Client(void);
	Client & operator=(const Client &);

	/* parser */
	void build_request(const std::string &);
	/* builder */
	void build_response(void);
	void what(void) const;

	std::string req;
	std::string rep;
};


