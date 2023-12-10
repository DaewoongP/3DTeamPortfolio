#pragma once
#include "Quest.h"

BEGIN(Client)

class CQuest_Town final : public CQuest
{
private:
	explicit CQuest_Town(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CQuest_Town() = default;

public:
	void Set_Clear() { m_isClear = true; }
	virtual _bool Is_Finished() override;

public:
	HRESULT Initialize();
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Clear_Quest();

private:
	_bool		m_isClear = { false };

public:
	static CQuest_Town* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END