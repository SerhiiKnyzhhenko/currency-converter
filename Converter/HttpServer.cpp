#include "HttpServer.hpp"


void HttpServer::_setCertPath(const std::string& certPath) {
	certificatePath_ = certPath;
}
//------------------------------------------------------------------------------------------
void HttpServer::_setKeyPath(const std::string& keyPath) {
	keyPath_ = keyPath;
}
//------------------------------------------------------------------------------------------
void HttpServer::_setPathForCA(const std::string& caPath) {
	caPath_ = caPath;
}
//------------------------------------------------------------------------------------------
HttpServer::HttpServer(int port) : port_(port) {
	db = new dataBase();
	rates = new currencyRates();
}
//------------------------------------------------------------------------------------------
HttpServer::~HttpServer() {
	delete db;
	delete rates;
}
//------------------------------------------------------------------------------------------
bool HttpServer::_ssl_init() {

	SSL_library_init(); /* load encryption & hash algorithms for SSL */
	SSL_load_error_strings(); /* load the error strings for good error reporting */
	OpenSSL_add_all_algorithms();

	ssl_method_ = TLS_server_method();// create new server method
	ssl_context_ = SSL_CTX_new(ssl_method_);//create new context from method

	SSL_CTX_set_min_proto_version(ssl_context_, TLS1_3_VERSION);// Disable obsolete protocols

	//Checking ssl_context_
	if (ssl_context_ == NULL) {
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(ssl_context_);
		return false;
	}

	// set the local certificate from certFile
	if (SSL_CTX_use_certificate_file(ssl_context_, certificatePath_.c_str(), SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(ssl_context_);
		return false;
	}

	// set the local key from keyFile
	if (SSL_CTX_use_PrivateKey_file(ssl_context_, keyPath_.c_str(), SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(ssl_context_);
		return false;
	}

	// check private key and certificate
	if (!SSL_CTX_check_private_key(ssl_context_)) {
		std::cerr << "Private key does not match the certificate." << std::endl;
		SSL_CTX_free(ssl_context_);
		return false;
	}

	// set CA certificate from ca_certFile if not empty
	if (!caPath_.empty()) {
		if (SSL_CTX_load_verify_locations(ssl_context_, caPath_.c_str(), nullptr) == 0) {
			ERR_print_errors_fp(stderr);
			SSL_CTX_free(ssl_context_);
			return false;
		}
		//SSL_CTX_set_verify(ssl_context_, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
		SSL_CTX_set_verify(ssl_context_, SSL_VERIFY_NONE, nullptr); //// For test
		SSL_CTX_set_verify_depth(ssl_context_, 1);
	}
	
	return true;
}
//------------------------------------------------------------------------------------------
bool HttpServer::_socket_init() {
	try {
		socket_ = std::make_unique<Socket>(PF_INET, Socket::Type::TCP);

		sockaddr_in sa_serv{};
		sa_serv.sin_family = AF_INET;
		sa_serv.sin_addr.s_addr = INADDR_ANY;
		sa_serv.sin_port = htons(port_);	  /* Server Port number */

		socket_->bind(reinterpret_cast<const struct sockaddr*>(& sa_serv), sizeof(sa_serv));
		socket_->listen(backlog); //1000

		return true;
	}
	catch (const std::system_error& e) {
		std::cerr << "Socket error: " << e.what() << std::endl;
		socket_.reset();
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		socket_.reset();
		return false;
	}
}
//------------------------------------------------------------------------------------------
void HttpServer::_client_processing(int sock_cli, const std::string& client_ip) {

	SSL* ssl = SSL_new(ssl_context_);
	if (!ssl) {
		ERR_print_errors_fp(stderr);
		closesocket(sock_cli);
		return;
	}

	SSL_set_fd(ssl, sock_cli);

	int err = SSL_accept(ssl);
	if (err <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_free(ssl);
		closesocket(sock_cli);
		return;
	}

	char buf[1024];
	err = SSL_read(ssl, buf, sizeof(buf) - 1);

	if (err > 0) {
		buf[err] = '\0';
		_parsingRequest(buf);
		//printf("Received %d chars: '%s'\n", err, buf);
	
		// Формируем HTTP-ответ
		const char* response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: 12\r\n"
			"\r\n"
			"Hello, SSL!";

		// Отправка ответа
		SSL_write(ssl, response, strlen(response));
	}
	else {
		ERR_print_errors_fp(stderr);
	}

	SSL_shutdown(ssl);
	SSL_free(ssl);
	closesocket(sock_cli);
}
//------------------------------------------------------------------------------------------
bool HttpServer::start() {
	try {
		if (!_ssl_init()) {
			std::cerr << "problem with ssl init" << std::endl;
			return false;
		}

		if (!_socket_init()) {
			std::cerr << "problem with socket init" << std::endl;
			return false;
		}

		is_running_ = true;
		std::cout << "Server started on port " << port_ << std::endl;

		while (is_running_) {
			struct sockaddr_in sa_cli {};
			socklen_t client_len = sizeof(sa_cli);

			/* Socket for a TCP/IP connection is created */
			SOCKET sock_cli = accept(socket_->getFd(), (struct sockaddr*)&sa_cli, &client_len);

			if (!is_running_) break;


			if (sock_cli == INVALID_SOCKET) {
				throw std::system_error(
					WSAGetLastError(),
					std::system_category(),
					"accept() failed"
				);
			}

			std::thread([this, sock_cli, sa_cli]() {
				char client_ip[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &sa_cli.sin_addr, client_ip, INET_ADDRSTRLEN);

				std::cout << "Connection from " << client_ip
					<< ":" << ntohs(sa_cli.sin_port) << std::endl;

				_client_processing(sock_cli, client_ip);

				}).detach();
		}
	}
	catch (const std::system_error& e) {
		std::cerr << "System error: " << e.what() << std::endl;
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return false;
	}
}
//------------------------------------------------------------------------------------------
void HttpServer::_parsingRequest(const std::string& request) {
	//GET /convert?from=AMD&to=ANG&amount=1&date=2025-05-24&places=2 HTTP/1.1

	// Извлекаем первую строку до '\n'
	size_t end_line = request.find('\n');
	std::string first_line = request.substr(0, end_line);

	// Извлекаем часть URL после '?' (параметры)
	size_t path_start = first_line.find(' ');
	size_t query_start = first_line.find('?', path_start);
	std::string query = first_line.substr(query_start + 1);

	// Разбиваем параметры на пары ключ=значение
	std::unordered_map<std::string, std::string> params;
	std::istringstream iss(query);
	std::string pair;

	while (std::getline(iss, pair, '&')) {
		size_t delimiter = pair.find('=');
		if (delimiter != std::string::npos) {
			std::string key = pair.substr(0, delimiter);
			std::string value = pair.substr(delimiter + 1);
			params[key] = value;
		}
	}
	_processingParameters(params);
}

void HttpServer::_processingParameters(std::unordered_map<std::string, std::string>& params) {
	std::string from = "";
	std::string to = "";
	std::string amount = "";
	std::string date = "";

	if (params.find("from") != params.end())
		from = params["from"].substr(0, 3);
	if (params.find("to") != params.end()) 
		to = params["to"].substr(0, 3);
	if (params.find("amount") != params.end())
		amount = params["amount"];
	if (params.find("date") != params.end())
		date = params["date"];

	if (!db->сheckDataForDate(date)) {
		httpClient requestToApi;
		apiRequest target;
		requestToApi.get_json_body(target.custom_date_request(date));
	}

}