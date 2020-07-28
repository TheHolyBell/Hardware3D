#define FULL_WINTARD
#include "GDIPlusManager.h"
#include <algorithm>

namespace Gdiplus
{
	using std::min;
	using std::max;
}

#include <gdiplus.h>

ULONG_PTR GDIPlusManager::s_Token;
int GDIPlusManager::s_RefCount;

GDIPlusManager::GDIPlusManager()
{
	if (s_RefCount++ == 0)
	{
		Gdiplus::GdiplusStartupInput _input;
		_input.GdiplusVersion = 1;
		_input.DebugEventCallback = nullptr;
		_input.SuppressBackgroundThread = false;
		Gdiplus::GdiplusStartup(&s_Token, &_input, nullptr);
	}
}

GDIPlusManager::~GDIPlusManager()
{
	if (--s_RefCount == 0)
		Gdiplus::GdiplusShutdown(s_Token);
}
