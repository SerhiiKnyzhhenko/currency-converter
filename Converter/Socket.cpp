#include "Socket.hpp"

// Constructor: initializes Winsock and creates a socket
Socket::Socket(int domain = AF_INET, Type type = Type::TCP) {
	Socket::socket_init();

	int type_ = (type == Type::TCP) ? SOCK_STREAM : SOCK_DGRAM;
	if (type_ != SOCK_STREAM) {
		throw std::logic_error("listen() is for TCP sockets only");
	}

	socket_ = ::socket(domain, type_, 0);
	if (socket_ == INVALID_SOCKET) {
		throw std::system_error(
			WSAGetLastError(),
			std::system_category(),
			"socket() failed"
		);
	}
}
//------------------------------------------------------------------------------------------
// Destructor: closes the socket if valid
Socket::~Socket() {
	if (socket_ != INVALID_SOCKET)
		closesocket(socket_);
}
//------------------------------------------------------------------------------------------
// Initializes Winsock (called once)
void Socket::socket_init() {
	static bool initialize = false;
	if (!initialize) {
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw std::system_error(
				WSAGetLastError(),
				std::system_category(),
				"WSAStartup failed"
			);
		}
	}
	initialize = true;
}
//------------------------------------------------------------------------------------------
// Move constructor: transfers ownership
Socket::Socket(Socket&& other) noexcept : socket_(other.socket_) {
	other.socket_ = INVALID_SOCKET;
}
//------------------------------------------------------------------------------------------
// Swaps socket descriptors
void Socket::swap(Socket& other) noexcept {
	std::swap(socket_, other.socket_);
}
//------------------------------------------------------------------------------------------
// Move assignment operator
Socket& Socket::operator=(Socket&& other) noexcept {
	Socket temp(std::move(other)); 
	swap(temp);                    
	return *this;
}
//------------------------------------------------------------------------------------------
// Binds the socket to a local address
void Socket::bind(const sockaddr* addr, int addrlen) {
	if (::bind(socket_, addr, addrlen) == SOCKET_ERROR) {
		throw std::system_error(
			WSAGetLastError(),
			std::system_category(),
			"bind() failed"
		);
	}
}
//------------------------------------------------------------------------------------------
// Starts listening for connections (TCP only)
void Socket::listen(int backlog = 128) {
	if (::listen(socket_, backlog) == SOCKET_ERROR) {
		throw std::system_error(
			WSAGetLastError(),
			std::system_category(),
			"listen() failed"
		);
	}
}
		
