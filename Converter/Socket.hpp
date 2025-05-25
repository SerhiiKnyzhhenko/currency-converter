#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <winsock2.h>
#include <ws2tcpip.h>
#include <system_error>
#include <stdexcept>
#include <string>

#pragma comment(lib, "ws2_32.lib")


class Socket {
private: 
	SOCKET socket_ = INVALID_SOCKET;
		
public:
	enum class Type { TCP, UDP };

	Socket(int domain, Type);
	~Socket();

	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	Socket(Socket&& other) noexcept;

	Socket& operator=(Socket&& other) noexcept;

	static void socket_init();
	void bind(const sockaddr* adrr, int adrrLen);
	void listen(int backlog);

	void swap(Socket& other) noexcept;

	SOCKET getFd() const noexcept { return socket_; }
};

#endif // !SOCKET_HPP