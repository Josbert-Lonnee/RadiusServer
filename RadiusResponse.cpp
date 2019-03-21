#include "stdafx.h"

// Interface Implemented:
#include "RadiusResponse.h"

#include "md5.h"

#include "RadiusSocket.h"
#include "RadiusError.h"


void RadiusResponse::initialize(Byte code)
{
	data.header.code = code;
	length = sizeof(data.header);
}

void RadiusResponse::addNameAttribute(const char* name)
{
	size_t nameLength = strlen(name);
	size_t attributeLength = sizeof(RaduisMessageAttributeHeader) + nameLength;
	_ASSERTE(attributeLength < 256);
	_ASSERTE(length + attributeLength < sizeof(data.buffer));

	RaduisMessageAttributeHeader* attributeHeader = (RaduisMessageAttributeHeader*)(data.buffer + length);
	attributeHeader->type = RADIUS_MESSAGE_ATTRIBUTE_TYPE_USER_NAME;
	attributeHeader->length = (Byte)attributeLength;
	memcpy(data.buffer + length + sizeof(RaduisMessageAttributeHeader), name, nameLength);

	length += attributeLength;
}

void RadiusResponse::deriveFromRequestAndFinalize(const RadiusMessage& radiusRequest)
{
	address = radiusRequest.getAddress();

	const Data& requestData = radiusRequest.getData();

	data.header.identifier = requestData.header.identifier;

	_ASSERTE(length < USHRT_MAX);
	data.header.length = _byteswap_ushort((unsigned short)length);

	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, (unsigned char*)data.buffer, 4);
	MD5Update(&md5, (unsigned char*)requestData.header.authenticator, sizeof(requestData.header.authenticator));
	MD5Update(&md5, (unsigned char*)(data.buffer + sizeof(data.header)), (unsigned)(length - sizeof(data.header)));
	MD5Update(&md5, (unsigned char*)SHARED_SECRET, SHARED_SECRET_LENGTH);
	MD5Final(&md5);

	memcpy(data.header.authenticator, md5.digest, 16);
}

void RadiusResponse::sendTo(const RadiusSocket& radiusSocket) const
{
	_ASSERTE( length <= sizeof(data.buffer) );

	int error = radiusSocket.send((const char*)data.buffer, length, (sockaddr*)&address);
	if (error)
		throwError(error, "sending message");
}
