#pragma once

#include <winsock.h>


class RadiusMessage {
protected:
	sockaddr_in address;

	size_t length = 0;

	union Data {
		Byte buffer[1024];

		struct Header {
			Byte code;
			Byte identifier;
			Word length; // Big-endian! Not specified in RFC 2865!
			Byte authenticator[16];
		} header;
	} data;

public:
	const sockaddr_in& getAddress() const { return address; }
	const Data&        getData   () const { return data;    }
};

struct RaduisMessageAttributeHeader {
	Byte type;
	Byte length;
};

#define SHARED_SECRET "radiussecret"
#define SHARED_SECRET_LENGTH 12

#define RADIUS_MESSAGE_CODE_ACCESS_REQUEST				 1
#define RADIUS_MESSAGE_CODE_ACCESS_ACCEPT				 2
#define RADIUS_MESSAGE_CODE_ACCESS_REJECT				 3
#define RADIUS_MESSAGE_CODE_ACCESS_CHALLENGE			11

#define RADIUS_MESSAGE_ATTRIBUTE_TYPE_USER_NAME			 1
#define RADIUS_MESSAGE_ATTRIBUTE_TYPE_USER_PASSWORD		 2
