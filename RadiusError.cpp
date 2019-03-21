#include "stdafx.h"

// Interface Implemented:
#include "RadiusError.h"


void throwError(int error, const char* action /*= nullptr*/)
{
	char* buffer = new char[1024];
	if (action == nullptr)
		sprintf_s(buffer, 1024, "Error: %d\n", error);
	else
		sprintf_s(buffer, 1024, "Error %s: %d\n", action, error);

	throw buffer;
}

void throwError(const char* error, const char* cause /*= nullptr*/)
{
	_ASSERTE( error != nullptr );

	char* buffer = new char[1024];

	if (cause == nullptr)
		sprintf_s(buffer, 1024, "Error when %s.\n", error);
	else
		sprintf_s(buffer, 1024, "Error when %s: %s\n", error, cause);

	throw buffer;
}
