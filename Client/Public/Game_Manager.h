#pragma once
/* =============================================== */
//	[CGame_Manager]
//	: 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Base.h"
#include "Client_Defines.h"
#include <any>

BEGIN(Client)

class CGame_Manager final : public CBase
{
	DECLARE_SINGLETON(CGame_Manager)

public:

private:

public:
	virtual void Free() override;
};

END