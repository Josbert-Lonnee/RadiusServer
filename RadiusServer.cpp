#include "stdafx.h"

#include "RadiusSocket.h"
#include "RadiusRequest.h"
#include "RadiusResponse.h"

#include "MariaDb.h"
#include "MariaDbPreparedStatement.h"

#include "RadiusError.h"

#define RADIUS_AUTHENTICATION_PORT 1812


int _tmain(int argc, _TCHAR* argv[])
{
	try {
		MariaDb mariaDb("localhost", "root", "root123", "radius");

		MariaDbPreparedStatement preparedStatement(mariaDb, "select Id, FullName from User where Name=? and Passwrd=password(?)");

		int userId;
		char userFullName[1024];

		preparedStatement.bindResult(0, &userId);
		preparedStatement.bindResult(1, userFullName, sizeof(userFullName));

		RadiusSocket radiusSocket(RADIUS_AUTHENTICATION_PORT);
		int error = radiusSocket.initialize();
		if (error)
			return -1;

		RadiusRequest request;
		RadiusResponse response;

		for (;;) {
			try {
				request.receiveFrom(radiusSocket);

				preparedStatement.bindParameter(0, request.getUserName());
				preparedStatement.bindParameter(1, request.getUserPassword());

				preparedStatement.executeWithResult();

				response.initialize(preparedStatement.next() ? RADIUS_MESSAGE_CODE_ACCESS_ACCEPT : RADIUS_MESSAGE_CODE_ACCESS_REJECT);
				response.addNameAttribute(userFullName);
				response.deriveFromRequestAndFinalize(request);
				response.sendTo(radiusSocket);
			}
			CATCH_RADIUS_ERROR;
		}
	}
	CATCH_RADIUS_ERROR;
}
