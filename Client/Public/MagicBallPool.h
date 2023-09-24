#pragma once
#include "Component.h"
#include "MagicBall.h"
#include "Client_Defines.h"
#include "Magic.h"

// �̱������� �Ҷ� �ϴٰ�
// �׳� renderó�� clone�� ���簡 �ƴ� ref������ ó������.
BEGIN(Client)

class CMagicBallPool final : public CComponent
{
public:
	explicit CMagicBallPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMagicBallPool() = default; 
	
public:
	virtual HRESULT Initialize_Prototype() override;

	CMagicBall* Get_Magic(CMagicBall::MAGICBALLINITDESC& MagicBallDesc);
	void Return_Magic(CMagicBall* pMagic , SPELL eSpell);

private:
	queue<CMagicBall*>	 m_MagicPoolVec[SPELL_END] = {};

private:
	CMagicBall* Create_Magic(SPELL eSpell, CMagicBall::MAGICBALLINITDESC& MagicBallDesc);
	void		Create_InitMagic(SPELL eSpell, const _tchar* szTagName, _uint iNumPool = 10);

public:
	static CMagicBallPool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
