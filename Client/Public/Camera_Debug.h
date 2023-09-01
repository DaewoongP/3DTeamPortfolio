#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCamera_Debug final : public CGameObject
{
private:
	explicit CCamera_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Debug(const CCamera_Debug& rhs);
	virtual ~CCamera_Debug() = default;

public:
	_float Get_Speed() { return m_pTransform->Get_Speed(); }
	_bool* Get_FixMouse() { return &m_isFixMouse; }
	void Set_Speed(_float _fSpeed) { m_pTransform->Set_Speed(_fSpeed); }
	void LookAt(_float3 _vTarget) { m_pTransform->LookAt(_vTarget); }
	void Set_Position(_float3 _vPosition) { m_pTransform->Set_Position(_vPosition); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

private:
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(_float fTimeDelta);
	void		Fix_Mouse(void);

private:
	_bool		m_isFixMouse = { 0 };
	_float		m_fSpeed = { 0.f };

public:
	static CCamera_Debug* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END