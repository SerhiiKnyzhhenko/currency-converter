#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <winsock2.h>
#include <ws2tcpip.h>
#include <system_error>
#include <stdexcept>
#include <string>

#pragma comment(lib, "ws2_32.lib")

// Wrapper class for Windows socket operations
class Socket {
private: 
	SOCKET socket_ = INVALID_SOCKET; // Underlying socket descriptor
		
public:
	enum class Type { TCP, UDP }; // Socket type (TCP or UDP)

	// Constructor: creates a socket with specified domain (AF_INET/AF_INET6) and type
	Socket(int domain, Type);
	~Socket(); // Destructor: closes the socket

	// Disable copy semantics
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	// Move semantics
	Socket(Socket&& other) noexcept;
	Socket& operator=(Socket&& other) noexcept;

	static void socket_init(); // Initializes Winsock library (called once per application)
	void bind(const sockaddr* adrr, int adrrLen);  // Binds the socket to a local address
	void listen(int backlog);  // Starts listening for incoming connections (TCP only)

	// Swaps socket descriptors between two Socket objects
	void swap(Socket& other) noexcept;

	// Returns the raw socket descriptor
	SOCKET getFd() const noexcept { return socket_; }
};

#endif // !SOCKET_HPP