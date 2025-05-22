#include "HttpServer.hpp"

static int backlog = 1000;

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

	socket_ = Socket(PF_INET, Socket::Type::TCP);
	CHK_ERR(socket_, "socket"); // check error

	sockaddr_in sa_serv{};
	sa_serv.sin_family = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port = htons(port_);	  /* Server Port number */

	if (socket_.bind((struct sockaddr*)&sa_serv, sizeof(sa_serv)) != 0) {
		std::cerr << "Problem with port binding" << std::endl;
		return false;
	}

	if (socket_.listen(backlog) != 0) {  // default 1000
		std::cerr << "Problem with port listening" << std::endl;
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------

void HttpServer::_client_processing(int, const std::string& ) {

}

//------------------------------------------------------------------------------------------

int HttpServer::start() {

}