#include "..\Public\PXAssertHandler.h"

void CPXAssertHandler::operator()(const char* exp, const char* file, int line, bool& ignore)
{
	cout << exp << endl;
	cout << file << endl;
	cout << line << endl;

	ignore = false;
}
