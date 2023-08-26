#pragma once
namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	const float g_fFrame = 60.f;

	enum LEVELID { LEVEL_TOOL, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Tool;