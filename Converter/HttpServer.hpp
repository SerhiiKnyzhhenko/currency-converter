#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP
#define CHK_ERR(expr, msg) 

#include <iostream>
#include <vector>
#include <fstream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "Socket.hpp"
#include <format>
#include <thread>
#include <memory>

class HttpServer {
private: 
	int port_{ 443 };
	int backlog = 1000;
	std::atomic<bool> is_running_{ false };

	std::unique_ptr<Socket> socket_;

	std::string certificatePath_;
	std::string keyPath_;
	std::string caPath_;
	std::string htmlData_;

	std::string startDateTime_;
	size_t getRequestCount_{ 0 };

	SSL* ssl { nullptr };
	const SSL_METHOD* ssl_method_{ nullptr };
	SSL_CTX* ssl_context_{ nullptr };

	std::vector<char> fav_icon_buffer;

private:
	bool _ssl_init();
	bool _socket_init();
	void _client_processing(int, const std::string&);

public:
	HttpServer(int port = 443);
	~HttpServer();

	void _setCertPath(const std::string&);
	void _setKeyPath(const std::string&);
	void _setPathForCA(const std::string&);

	bool start();
};

#endif // !HTTP_SERVER_HPP



