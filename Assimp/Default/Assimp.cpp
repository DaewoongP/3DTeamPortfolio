﻿#include "MainConverter.h"

int main()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

    _tchar szFilePath[MAX_STR] = TEXT("");
    _uint iConvertType = { 0 };
    wcout << TEXT("Input Convert Type") << endl;

    wcout << TEXT("0. NonAnim | 1. Anim | 2. MapObj | 3. OnlyAnim : ");

    wcin >> iConvertType;

    wcout << TEXT("Input File Path : ");
    wcin >> szFilePath;

    CMainConverter::Convert(iConvertType, szFilePath);

    return 0;
}
