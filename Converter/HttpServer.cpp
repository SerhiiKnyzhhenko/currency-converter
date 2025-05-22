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

HttpServer::HttpServer(int port) {

}

//------------------------------------------------------------------------------------------

HttpServer::~HttpServer() {

}

//------------------------------------------------------------------------------------------

bool HttpServer::_ssl_init() {

	SSL_library_init(); /* load encryption & hash algorithms for SSL */
	SSL_load_error_strings(); /* load the error strings for good error reporting */

	ssl_method_ = TLS_server_method();// create new server method
	ssl_context_ = SSL_CTX_new(ssl_method_);//create new context from method

	SSL_CTX_set_min_proto_version(ssl_context_, TLS1_3_VERSION);// Disable obsolete protocols

	//Checking ssl_context_
	if (ssl_context_ == NULL) {
		ERR_print_errors_fp(stderr);
		return false;
	}

	// set the local certificate from certFile
	if (SSL_CTX_use_certificate_file(ssl_context_, certificatePath_.c_str(), SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		return false;
	}

	// set the local key from keyFile
	if (SSL_CTX_use_PrivateKey_file(ssl_context_, keyPath_.c_str(), SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		return false;
	}

	if (caPath_.length() > 0) {
		// set CA certificate from ca_certFile
		if (SSL_CTX_load_verify_locations(ssl_context_, caPath_.c_str(), NULL) == 0) {
			ERR_print_errors_fp(stderr);
			return false;
		}
		// verification 
		SSL_CTX_set_verify(ssl_context_, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
		// Set the verification depth to 1
		SSL_CTX_set_verify_depth(ssl_context_, 1);
	}
	
	//A newly created SSL structure inherits information from the SSL_CTX structure.
	//This information includes types of connection methods, options, verification settings, and timeout settings.
	ssl = SSL_new(ssl_context_);

	return true;
}

//------------------------------------------------------------------------------------------

bool HttpServer::_socket_init() {
	try {
		socket_ = Socket(PF_INET, Socket::Type::TCP);

		sockaddr_in sa_serv{};
		sa_serv.sin_family = AF_INET;
		sa_serv.sin_addr.s_addr = INADDR_ANY;
		sa_serv.sin_port = htons(port_);	  /* Server Port number */

		socket_.bind((struct sockaddr*)&sa_serv, sizeof(sa_serv));
		socket_.listen(backlog); //1000

		struct sockaddr_in sa_cli{};
		socklen_t client_len = sizeof(sa_cli);

		/* Socket for a TCP/IP connection is created */
		SOCKET sock = accept(socket_.getFd(), (struct sockaddr*)&sa_cli, &client_len);

		if (sock == INVALID_SOCKET) {
			throw std::system_error(
				WSAGetLastError(),
				std::system_category(),
				"accept() failed"
			);
		}

		std::cout << std::format("Connection from {}, port {}", sa_cli.sin_addr.s_addr,
			sa_cli.sin_port) << std::endl;

		return true;
	}
	catch (const std::system_error& e) {
		std::cerr << "Socket error: " << e.what() << std::endl;
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return false;
	}
}

//------------------------------------------------------------------------------------------

void HttpServer::_client_processing(int, const std::string& ) {

}

//------------------------------------------------------------------------------------------

int HttpServer::start() {

}