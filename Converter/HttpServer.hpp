#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#define CHK_ERR(expr, msg) 
#define RETURN_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); }

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <format>
#include <thread>
#include <memory>
#include <unordered_map>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "Socket.hpp"
#include "dataBase.hpp"
#include "currencyRates.hpp"
#include "httpClient.hpp"
#include "apiRequest.hpp"
#include "jsonParser.hpp"


struct SSL_CTX_Deleter {
	void operator()(SSL_CTX* ssl_ctx) const {
		SSL_CTX_free(ssl_ctx);
	}
};

struct SSL_Deleter {
	void operator()(SSL* ssl) const {
		SSL_free(ssl);
	}
};

using SSL_CTX_ptr = std::unique_ptr<SSL_CTX, SSL_CTX_Deleter>;
using SSL_ptr = std::unique_ptr<SSL, SSL_Deleter>;

class HttpServer {
private:
	std::unique_ptr<dataBase> db;
	std::unique_ptr<currencyRates> rates;

	int port_{ 443 };
	int backlog = 1000;
	std::atomic<bool> is_running_{ false };

	std::unique_ptr<Socket> socket_;

	std::string certificatePath_ = "";
	std::string keyPath_ = "";
	std::string caPath_ = "";
	std::string htmlData_;

	std::string startDateTime_;
	size_t getRequestCount_{ 0 };

	const SSL_METHOD* ssl_method_{ nullptr };
	SSL_CTX_ptr ssl_context_;

	std::vector<char> fav_icon_buffer;

private:
	void _ssl_init();
	void _socket_init();
	void _client_processing(int, const std::string&);

	void readErrorCode(int err);

	void _setCertPath(const std::string&);
	void _setKeyPath(const std::string&);
	void _setPathForCA(const std::string&);

	double _parsingRequest(const std::string& request);
	double _processingParameters(std::unordered_map<std::string, std::string>& params);

public:
	HttpServer(int port = 443);
	~HttpServer();

	bool start();
};

#endif // !HTTP_SERVER_HPP



