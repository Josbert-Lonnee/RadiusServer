#include "stdafx.h"

// Interface Implemented:
#include "RadiusRequest.h"

#include "md5.h"

#include "RadiusSocket.h"
#include "RadiusError.h"


void RadiusRequest::receiveFrom(const RadiusSocket& socket)
{
	clear();

	int error = socket.receive((char*)data.buffer, sizeof(data), (sockaddr*)&address, &length);
	if (error)
		throwError(error, "receiving data");

	// Minimal length of a RADIUS package, the length of one containing no parameters at all:
	if (length < sizeof(RadiusMessage::Data::Header))
		throwError("Received message shorter than a single header.");

	// Package larger then the buffer??
	if (length >= sizeof(data))
		throwError("Received message size exceeds buffer size!");

	unsigned short dataLength = _byteswap_ushort(data.header.length);

	// Minimal length of a RADIUS package, the length of one containing no parameters at all:
	if (length < sizeof(RadiusMessage::Data::Header))
		throwError("Received message with alleged size smaller than a single header.");

	// Package smaller then the data length allegedly send?
	if (dataLength > length)
		throwError("Received message with alleged size larger than actual size.");

	// Attributes:
	unsigned short attributesLength = dataLength - sizeof(RadiusMessage::Data::Header);
	const Byte* attributeData = data.buffer + sizeof(RadiusMessage::Data::Header);
	while (attributesLength > 0) {
		unsigned n = interpretAttribute(attributesLength, (const RaduisMessageAttributeHeader*)attributeData);
		attributesLength -= n;
		attributeData += n;
	}

	printf("Received message from port %hu of size %hu...\n", _byteswap_ushort(address.sin_port), dataLength);
}

unsigned RadiusRequest::interpretAttribute(unsigned short lengthLeft, const RaduisMessageAttributeHeader* attributeHeader)
{
	_ASSERTE(attributeHeader != nullptr);

	if (lengthLeft == 0)
		return 0; // Nothing left

	if (lengthLeft < 2)
		throwError("Attribute rudiment encounted.");
	if (attributeHeader->length > lengthLeft)
		throwError("Attribute overlapping actual end encounted.");

	const Byte *attributeValue = ((const Byte*)attributeHeader) + sizeof(RaduisMessageAttributeHeader);

	switch (attributeHeader->type) {
	case RADIUS_MESSAGE_ATTRIBUTE_TYPE_USER_NAME:
		_ASSERTE(attributeHeader->length > sizeof(RaduisMessageAttributeHeader) + 1);
		_ASSERTE( _userName.empty() );
		_userName.assign((const char*)attributeValue, attributeHeader->length - sizeof(RaduisMessageAttributeHeader));
		break;

	case RADIUS_MESSAGE_ATTRIBUTE_TYPE_USER_PASSWORD: {
		unsigned short attributeDataLeft = attributeHeader->length;

		_ASSERTE( attributeDataLeft > 3 );

		attributeDataLeft -= 2;

		_ASSERTE( (attributeDataLeft & 0x7) == 0 );
		_ASSERTE( _userPassword.empty() );

		MD5_CTX md5;

		MD5Init(&md5);
		MD5Update(&md5, (unsigned char*)SHARED_SECRET, SHARED_SECRET_LENGTH);
		MD5Update(&md5, (unsigned char*)data.header.authenticator, sizeof(data.header.authenticator));
		MD5Final(&md5);

		for (; attributeDataLeft > 0;) {

			Byte p[16], *p_p = p;
			const Byte *p_digest = md5.digest, *p_data2 = attributeValue;
			for (unsigned i = 0; i < 16; ++i, ++p_p, ++p_data2, ++p_digest)
				*p_p = *p_data2 ^ *p_digest;

			_userPassword.append((const char*)p, 16);
			attributeDataLeft -= sizeof(data.header.authenticator);

			if (attributeDataLeft == 0)
				break;

			MD5Init(&md5);
			MD5Update(&md5, (unsigned char*)SHARED_SECRET, SHARED_SECRET_LENGTH);
			MD5Update(&md5, (unsigned char*)attributeValue, 16);
			MD5Final(&md5);

			attributeValue += sizeof(data.header.authenticator);
		}
			
		break; }
	default:
		throwError("Unimplemented or unknown attribute type encounted.");
	}

	return attributeHeader->length;
}

void RadiusRequest::clear()
{
	_userName    .clear();
	_userPassword.clear();
}
