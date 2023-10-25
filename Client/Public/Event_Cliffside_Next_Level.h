#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTrigger;
class CCliff_Gate;

class CEvent_Cliffside_Next_Level final : public CGameObject
{
private:
	explicit CEvent_Cliffside_Next_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Cliffside_Next_Level(const CEvent_Cliffside_Next_Level& rhs);
	virtual ~CEvent_Cliffside_Next_Level() = default;

public:
	_bool Get_Level_Change() const { return m_isCheck; } // 플레이어와 충돌하면 true를 반환

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger*	m_pNext_Level = { nullptr };

	CCliff_Gate* m_pCliff_Gate = { nullptr };

private:
	_bool	m_isCheck = { false };

private:
	HRESULT Add_Components();
	void	Check_Event(_float fTimeDelta);

public:
	static CEvent_Cliffside_Next_Level* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END