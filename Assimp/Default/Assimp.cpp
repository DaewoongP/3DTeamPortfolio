#include "MainConverter.h"

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

    _tchar szFilePath[MAX_STR] = TEXT("");
    wcout << TEXT("Input File Path : ");
    wcin >> szFilePath;

    CMainConverter::Convert(szFilePath);
}
