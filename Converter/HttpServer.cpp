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

}

//------------------------------------------------------------------------------------------

bool HttpServer::_socket_init() {

}

//------------------------------------------------------------------------------------------

void HttpServer::_client_processing(const std::string& ) {

}

//------------------------------------------------------------------------------------------

int HttpServer::start() {

}