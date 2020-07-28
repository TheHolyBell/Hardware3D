#pragma once
#include "ChiliWin.h"

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR s_Token;
	static int s_RefCount;
};