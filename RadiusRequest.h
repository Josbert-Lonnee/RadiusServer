#pragma once

#include <string>

#include "RadiusMessage.h"

class RadiusSocket;

class RadiusRequest : public RadiusMessage
{
	std::string		_userName, _userPassword;

public:
	void receiveFrom(const RadiusSocket& socket);

	const char* getUserName() const { return _userName.c_str(); }
	const char* getUserPassword() const { return _userPassword.c_str(); }

private:
	unsigned interpretAttribute(unsigned short lengthLeft, const RaduisMessageAttributeHeader* attributeHeader);
	void clear();
};
