#include "stdafx.h"

// Interface Implemented:
#include "RadiusSocket.h"


RadiusSocket::RadiusSocket(unsigned short port)
: _initializationPhase(NOT_INITIALIZED)
, _port(port)
,	_socket(INVALID_SOCKET)
{}

int RadiusSocket::initialize()
{
	_ASSERTE(_initializationPhase == NOT_INITIALIZED);

	WORD wVersionRequested = MAKEWORD(1, 1);
	int error = WSAStartup(wVersionRequested, &_wsaData);
	if (error)
		return error;

	_initializationPhase = WSA_INITIALIZED;

	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_socket == INVALID_SOCKET) {
		error = WSAGetLastError();
		return error;
	}

	_initializationPhase = SOCKET_INITIALIZED;

	_inboundSocketAddress.sin_family = AF_INET;
	_inboundSocketAddress.sin_port = htons(_port);
	_inboundSocketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	_ASSERTE( sizeof(long long) == 8 );
	*((long long*)_inboundSocketAddress.sin_zero) = 0;

	if (bind(_socket, (SOCKADDR *)&_inboundSocketAddress, sizeof _inboundSocketAddress) == SOCKET_ERROR) {
		error = WSAGetLastError();
		return error;
	}

	_initializationPhase = SOCKET_BOUND;
	return 0; // Success
}

int RadiusSocket::receive(char* buffer, size_t bufferSize, sockaddr* address, size_t* lenghtReceived) const
{
	_ASSERTE( buffer != nullptr );
	_ASSERTE( address != nullptr );
	_ASSERTE( lenghtReceived != nullptr );

	if (_initializationPhase < SOCKET_BOUND)
		return -1;

	int addressLength = sizeof(*address);
	int n = ::recvfrom(_socket, buffer, (int)bufferSize, 0, address, &addressLength);
	if (n == SOCKET_ERROR) {
		int error = WSAGetLastError();
		return error;
	}
	_ASSERTE( addressLength == sizeof(*address) );
	_ASSERTE(((sockaddr_in*)address)->sin_family == AF_INET);

	*lenghtReceived = n;
	return 0; // Success
}

int RadiusSocket::send(const char* buffer, size_t bufferSize, sockaddr* address) const
{
	_ASSERTE( buffer != nullptr );
	_ASSERTE( address != nullptr );

	if (_initializationPhase < SOCKET_BOUND)
		return -1;

	int n = ::sendto(_socket, buffer, (int)bufferSize, 0, (sockaddr*)address, sizeof(*address));
	if (n == SOCKET_ERROR) {
		int error = WSAGetLastError();
		return error;
	}

	return 0; // Success
}

RadiusSocket::~RadiusSocket()
{
	if (_initializationPhase >= SOCKET_INITIALIZED && _socket != INVALID_SOCKET)
		closesocket(_socket);

	if (_initializationPhase >= NOT_INITIALIZED)
		WSACleanup();
}