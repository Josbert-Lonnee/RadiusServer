#pragma once

#include <winsock.h>

class RadiusSocket {
	enum InitilizationPhase { NOT_INITIALIZED, WSA_INITIALIZED, SOCKET_INITIALIZED, SOCKET_BOUND };

private:
	InitilizationPhase		_initializationPhase;
	unsigned short			_port;
	WSADATA					_wsaData;
	SOCKET					_socket;
	struct sockaddr_in		_inboundSocketAddress;

public:
	RadiusSocket(unsigned short port);
	int initialize();
	int receive(char* buffer, size_t bufferSize, sockaddr* address, size_t* lenghtReceived) const;
	int send(const char* buffer, size_t bufferSize, sockaddr* address) const;
	~RadiusSocket();
};
