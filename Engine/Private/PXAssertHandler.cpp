#include "..\Public\PXAssertHandler.h"

void CPXAssertHandler::operator()(const char* exp, const char* file, int line, bool& ignore)
{
	ignore = false;
}
