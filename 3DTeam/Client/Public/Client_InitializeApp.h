#pragma once
namespace Client
{
}
#ifdef _DEBUG
void SetConsoleWindowSize(_int width, _int height)
{
	// 콘솔 핸들 얻음
	_tchar consoleTitle[MAX_PATH];
	GetConsoleTitle(consoleTitle, MAX_PATH);
	HWND console = FindWindow(NULL, consoleTitle);

	// 콘솔 크기 조절
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, width, height, TRUE);
}

void SetConsoleWindowPosition(_int x, _int y)
{
	// 콘솔 핸들 얻음
	_tchar consoleTitle[MAX_PATH];
	GetConsoleTitle(consoleTitle, MAX_PATH);
	HWND console = FindWindow(NULL, consoleTitle);

	// 콘솔 위치 조절
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, x, y, ConsoleRect.right - ConsoleRect.left, ConsoleRect.bottom - ConsoleRect.top, TRUE);
}

#endif //_DEBUG