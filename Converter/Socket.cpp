#include "Socket.hpp"

Socket::Socket(int domain = AF_INET, Type type = Type::TCP) {
	socket_init();

	int type_ = (type == Type::TCP) ? SOCK_STREAM : SOCK_DGRAM;
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

Socket::~Socket() {
	if (socket_ != INVALID_SOCKET)
		closesocket(socket_);
}

//------------------------------------------------------------------------------------------

void Socket::socket_init() {
	static bool initialize = false;
	if (!initialize) {
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw std::runtime_error("WSAStartup failed");
		}
	}
	initialize = true;
}

//------------------------------------------------------------------------------------------

Socket::Socket(Socket&& other) noexcept : socket_(other.socket_) {
	other.socket_ = INVALID_SOCKET;
}

//------------------------------------------------------------------------------------------

void Socket::swap(Socket& other) noexcept {
	using std::swap;
	swap(socket_, other.socket_);
	swap(socket_, other.socket_);
}

//------------------------------------------------------------------------------------------

Socket& Socket::operator=(Socket&& other) noexcept {
	Socket temp(std::move(other)); 
	swap(temp);                    
	return *this;
}

//------------------------------------------------------------------------------------------

bool Socket::bind(const sockaddr* addr, int addrlen) {
	if (::bind(socket_, addr, addrlen) == SOCKET_ERROR)
		return false;

	return true;
}

//------------------------------------------------------------------------------------------

bool Socket::listen(int backlog = 128) {
	if (::listen(socket_, backlog) == SOCKET_ERROR)
		return false;

	return true;
}
