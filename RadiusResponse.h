#pragma once

#include "RadiusMessage.h"

class RadiusSocket;

class RadiusResponse : public RadiusMessage
{
public:
	void initialize(Byte code);
	void addNameAttribute(const char* name);
	void deriveFromRequestAndFinalize(const RadiusMessage& radiusRequest);
	void sendTo(const RadiusSocket& radiusSocket) const;
};
