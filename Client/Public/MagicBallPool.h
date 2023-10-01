#pragma once
#include "Base.h"
#include "MagicBall.h"
#include "Client_Defines.h"
#include "Magic.h"

BEGIN(Client)

class CMagicBallPool final : public CBase
{
	DECLARE_SINGLETON(CMagicBallPool)
public:
	explicit CMagicBallPool() = default;
	virtual ~CMagicBallPool() = default; 
	
public:
	HRESULT Initialize();

public:
	CMagicBall* Get_Magic(CMagicBall::MAGICBALLINITDESC& MagicBallDesc);
	void Return_Magic(CMagicBall* pMagic, SPELL eSpell);

private:
	queue<CMagicBall*>	 m_MagicPool[SPELL_END] = {};

private:
	CMagicBall* Create_Magic(SPELL eSpell);
	void		Create_InitMagic(SPELL eSpell, const _tchar* szTagName, _uint iNumPool = 10);

public:
	virtual void Free() override;
};

END
