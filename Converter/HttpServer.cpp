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
	try {
		db = std::make_unique<dataBase>();
		rates = std::make_unique<currencyRates>();
	}
	catch (const std::exception& e) {
		std::cerr << "[ERROR] Initialization failed: " << e.what() << std::endl;
		throw std::runtime_error("Critical initialization error");
	}
}
//------------------------------------------------------------------------------------------
HttpServer::~HttpServer() {
	std::cout << "Server stopped\n";
	std::cout << "Total connections handled: " << getRequestCount_ << "\n";
}
//------------------------------------------------------------------------------------------
void HttpServer::_ssl_init() {

	if (OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | 
						 OPENSSL_INIT_LOAD_CRYPTO_STRINGS, 
						 NULL) != 1) {
		throw std::runtime_error("OPENSSL_init_ssl() failed" + std::to_string(ERR_get_error()));
	}
		
	if (OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS | 
							OPENSSL_INIT_ADD_ALL_CIPHERS | 
							OPENSSL_INIT_ADD_ALL_DIGESTS, 
							NULL) != 1) {
		throw std::runtime_error("OPENSSL_init_crypto() failed" + std::to_string(ERR_get_error()));
	}


	ssl_method_ = TLS_server_method();// create new server method
	ssl_context_ = SSL_CTX_ptr(SSL_CTX_new(ssl_method_));//create new context from method

	//Checking ssl_context_
	if (!ssl_context_)
		throw std::runtime_error("SSL_CTX_new() failed" + std::to_string(ERR_get_error()));

	SSL_CTX_set_cipher_list(ssl_context_.get(), "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256");
	SSL_CTX_set_min_proto_version(ssl_context_.get(), TLS1_3_VERSION);// Disable obsolete protocols


	// set the local certificate from certFile
	if (SSL_CTX_use_certificate_file(ssl_context_.get(), certificatePath_.c_str(), SSL_FILETYPE_PEM) <= 0) 
		throw std::runtime_error("Failed to load certificate key" + std::to_string(ERR_get_error()));

	// set the local key from keyFile
	if (SSL_CTX_use_PrivateKey_file(ssl_context_.get(), keyPath_.c_str(), SSL_FILETYPE_PEM) <= 0) 
		throw std::runtime_error("Failed to load private key" + std::to_string(ERR_get_error()));

	// check private key and certificate
	if (!SSL_CTX_check_private_key(ssl_context_.get())) 
		throw std::runtime_error("SSL_CTX_check_private_key() failed" + std::to_string(ERR_get_error()));

	// set CA certificate from ca_certFile if not empty
	if (!caPath_.empty()) {
		if (SSL_CTX_load_verify_locations(ssl_context_.get(), caPath_.c_str(), nullptr) == 0) {
			throw std::runtime_error("Failed to load CA certificates" + std::to_string(ERR_get_error()));
		}
		//SSL_CTX_set_verify(ssl_context_, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
		SSL_CTX_set_verify(ssl_context_.get(), SSL_VERIFY_NONE, nullptr); //// For test
		SSL_CTX_set_verify_depth(ssl_context_.get(), 1);
	}
}
//------------------------------------------------------------------------------------------
void HttpServer::_socket_init() {
	try {
		socket_ = std::make_unique<Socket>(PF_INET, Socket::Type::TCP);

		sockaddr_in sa_serv{};
		sa_serv.sin_family = AF_INET;
		sa_serv.sin_addr.s_addr = INADDR_ANY;
		sa_serv.sin_port = htons(port_);	  /* Server Port number */

		socket_->bind(reinterpret_cast<const struct sockaddr*>(& sa_serv), sizeof(sa_serv));
		socket_->listen(backlog); //1000
	}
	catch (const std::system_error& e) {
		std::cerr << "Socket error: " << e.what() << std::endl;
		socket_.reset();
		throw;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		socket_.reset();
		throw;
	}
}
//------------------------------------------------------------------------------------------
void HttpServer::_client_processing(int sock_cli, const std::string& client_ip) {

	SSL_ptr ssl(SSL_new(ssl_context_.get()));
	if (!ssl) {
		ERR_print_errors_fp(stderr);
		closesocket(sock_cli);
		return;
	}

	SSL_set_fd(ssl.get(), sock_cli);


	while (true) {
		int ret = SSL_accept(ssl.get());
		if (ret == 1) break; // Успех
		int error = SSL_get_error(ssl.get(), ret);
		if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE) {
			// Повторить SSL_accept позже.
			continue;
		}
		else {
			// Обработать ошибку.
			break;
		}
	}

	char buf[1024];
	int err = SSL_read(ssl.get(), buf, sizeof(buf) - 1);

	if (err > 0) {
		buf[err] = '\0';
		double result = _parsingRequest(buf);

		// Формируем JSON-ответ
		std::string json_response = "{\"result\": " + std::to_string(result) + "}";

		// Устанавливаем заголовки
		std::string headers =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json\r\n"
			"Access-Control-Allow-Origin: *\r\n" 
			"Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
			"Access-Control-Allow-Headers: Content-Type\r\n"
			"Content-Length: " + std::to_string(json_response.size()) + "\r\n"
			"\r\n";

		// Отправка заголовков и JSON-ответа
		int ret = SSL_write(ssl.get(), headers.c_str(), headers.size());
		if (ret <= 0)
			int error = SSL_get_error(ssl.get(), ret);

		SSL_write(ssl.get(), json_response.c_str(), json_response.size());
		if (ret <= 0)
			int error = SSL_get_error(ssl.get(), ret);
		

		std::cout << "Response sent to client:\n"
			<< headers << json_response << std::endl;
	}
	else {
		ERR_print_errors_fp(stderr);
	}

	SSL_shutdown(ssl.get());
	closesocket(sock_cli);
}
//------------------------------------------------------------------------------------------
bool HttpServer::start() {
	try {
		_setCertPath("C:/Users/12345/source/repos/Converter/Converter/certificates/certificate.crt");
		_setKeyPath("C:/Users/12345/source/repos/Converter/Converter/certificates/private.key");

		try {
			_ssl_init();
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] SSL init Error: " << e.what() << std::endl;
			throw;
		}

		try {
			_socket_init();
		}
		catch (const std::exception& e) {
			std::cerr << "[ERROR] Socket init Error: " << e.what() << std::endl;
			throw;
		}

		is_running_ = true;
		std::cout << "Server started on port " << port_ << std::endl;

		while (is_running_) {
			fd_set read_fds;
			FD_ZERO(&read_fds);
			FD_SET(socket_->getFd(), &read_fds);

			timeval timeout{ 1, 0 };
			int activity = select(socket_->getFd() + 1, &read_fds, nullptr, nullptr, &timeout);

			if (activity < 0) {
				throw std::system_error(WSAGetLastError(), std::system_category(), "select() failed");
			}
			else if (activity > 0) {
				struct sockaddr_in sa_cli {};
				socklen_t client_len = sizeof(sa_cli);
				SOCKET sock_cli = accept(socket_->getFd(), (struct sockaddr*)&sa_cli, &client_len);

				if (!is_running_) break;

				if (sock_cli == INVALID_SOCKET) {
					throw std::system_error(
						WSAGetLastError(),
						std::system_category(),
						"accept() failed");
				}
				else {
					std::thread([this, sock_cli, sa_cli]() {
						char client_ip[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, &sa_cli.sin_addr, client_ip, INET_ADDRSTRLEN);

						std::cout << "Connection from " << client_ip
							<< ":" << ntohs(sa_cli.sin_port) << std::endl;

						_client_processing(sock_cli, client_ip);
						getRequestCount_ += 1;
						}).detach();
				}
			}	
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
double HttpServer::_parsingRequest(const std::string& request) {
	//GET /convert?from=AMD&to=ANG&amount=1&date=2025-05-24&places=2 HTTP/1.1

	// Извлекаем первую строку до '\n'
	size_t end_line = request.find('\n');
	std::string first_line = request.substr(0, end_line);
	std::cout << first_line << std::endl;
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
	return _processingParameters(params);
}
//------------------------------------------------------------------------------------------
double HttpServer::_processingParameters(std::unordered_map<std::string, std::string>& params) {
	double result = 0;

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

	if (!db->checkDataForDate(date)) {
		try {
			httpClient requestToApi;
			apiRequest target;
			requestToApi.get_json_body(target.custom_date_request(date));
			jsonParser jPars;
			jPars.write_to_db(*db);
			jPars.write_to_hash(rates->get_rates());

			result = rates->convert(std::stod(amount), rates->get_rate(to), rates->get_rate(from));
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}	
	}
	else {
		db->add_resp_to_hash(date, rates->get_rates());
		result = rates->convert(std::stod(amount), rates->get_rate(to), rates->get_rate(from));
	}
		
	return result;
}
//------------------------------------------------------------------------------------------
void HttpServer::readErrorCode(int error) {

}