#pragma once
namespace Client
{
}
#ifdef _DEBUG
void SetConsoleWindowSize(_int width, _int height)
{
	// �ܼ� �ڵ� ����
	_tchar consoleTitle[MAX_PATH];
	GetConsoleTitle(consoleTitle, MAX_PATH);
	HWND console = FindWindow(NULL, consoleTitle);

	// �ܼ� ũ�� ����
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, width, height, TRUE);
}

void SetConsoleWindowPosition(_int x, _int y)
{
	// �ܼ� �ڵ� ����
	_tchar consoleTitle[MAX_PATH];
	GetConsoleTitle(consoleTitle, MAX_PATH);
	HWND console = FindWindow(NULL, consoleTitle);

	// �ܼ� ��ġ ����
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, x, y, ConsoleRect.right - ConsoleRect.left, ConsoleRect.bottom - ConsoleRect.top, TRUE);
}

#endif //_DEBUG