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
#include "currency_rates.hpp"
#include "converter.hpp"



class HttpServer {
private:
	dataBase* db;
	currencyRates* rates;

	int port_{ 443 };
	int backlog = 1000;
	std::atomic<bool> is_running_{ false };

	std::unique_ptr<Socket> socket_;

	std::string certificatePath_ = "C:/Users/12345/source/repos/Converter/Converter/certificates/certificate.crt";
	std::string keyPath_ = "C:/Users/12345/source/repos/Converter/Converter/certificates/private.key";
	std::string caPath_ = "";
	std::string htmlData_;

	std::string startDateTime_;
	size_t getRequestCount_{ 0 };

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

	void _parsingRequest(const std::string& request);
	void _processingParameters(std::unordered_map<std::string, std::string>& params);

	bool start();
};

#endif // !HTTP_SERVER_HPP



