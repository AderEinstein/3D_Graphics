#pragma once
#include "AderWin.h"

class GDIPlusManager
{
private:
	static ULONG_PTR token;
	static int refCount;
public:
	GDIPlusManager();
	~GDIPlusManager();
};