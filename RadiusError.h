#pragma once

void throwError(int error, const char* action = nullptr);
void throwError(const char* error, const char* cause = nullptr);

#define CATCH_RADIUS_ERROR \
catch (char* errorMessage) {\
	printf("%s", errorMessage);\
	delete[] errorMessage;\
}
