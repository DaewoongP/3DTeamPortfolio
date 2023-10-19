#pragma once
#include "Quest.h"

BEGIN(Client)

class CQuest_Save_Fig final : public CQuest
{
private:
	explicit CQuest_Save_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CQuest_Save_Fig() = default;

public:
	virtual _bool Is_Finished() override;

public:
	HRESULT Initialize();
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool		m_isSaved = { false };
	/*_uint		m_iNumGrass = 0;
	_uint		m_iNumTest = 0;
	_uint		m_iNumTest1 = 0;
	_uint		m_iNumTest2 = 0;*/

public:
	static CQuest_Save_Fig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END