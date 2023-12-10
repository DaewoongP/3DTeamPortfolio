#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTrigger;

class CEvent_Smeade_Next_Level final : public CGameObject
{
private:
	explicit CEvent_Smeade_Next_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Smeade_Next_Level(const CEvent_Smeade_Next_Level& rhs);
	virtual ~CEvent_Smeade_Next_Level() = default;

public:
	_bool Get_Level_Change_Cliff() const { return m_isCheck_Cliff; }		// 플레이어와 충돌하면
	_bool Get_Level_Change_Sanctum() const { return m_isCheck_Sanctum; }	// true를 반환
	_bool Get_Level_Change_Sky() const { return m_isCheck_Sky; }	// true를 반환

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	CTrigger* m_pNext_Level_Cliff = { nullptr };
	CTrigger* m_pNext_Level_Sanctum = { nullptr };
	CTrigger* m_pNext_Level_Sky = { nullptr };

private:
	_bool	m_isCheck_Cliff = { false };
	_bool	m_isCheck_Sanctum = { false };
	_bool	m_isCheck_Sky = { false };

private:
	HRESULT Add_Components();
	void	Check_Event_Cliff(_float fTimeDelta);
	void	Check_Event_Sanctum(_float fTimeDelta);
	void	Check_Event_Sky(_float fTimeDelta);

public:
	static CEvent_Smeade_Next_Level* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END