#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CPXAssertHandler : public cloth::PxAssertHandler
{
	virtual void operator()(const char* exp, const char* file, int line, bool& ignore) override;
};

END