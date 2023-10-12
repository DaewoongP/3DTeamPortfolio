#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRigidBody;
#ifdef _DEBUG
class CRenderer;
#endif // _DEBUG
END

BEGIN(Client)

class CTrigger final : public CGameObject
{
public:
	typedef struct tagTriggerDesc
	{
		_char szCollisionTag[MAX_PATH] = "";
		_tchar szOtherTag[MAX_PATH] = TEXT("");
		_float3	vTriggerSize;
		_float3	vTriggerWorldPos;
	}TRIGGERDESC;

private:
	explicit CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	_bool Is_Collision() { return m_isCollision; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;

private:
	CRigidBody*			m_pRigidBody = { nullptr };

private:
	_bool				m_isCollision = { false };
	_tchar				m_szOtherTag[MAX_PATH] = TEXT("");

#ifdef _DEBUG
private:
	CRenderer*			m_pRenderer = { nullptr };
#endif // _DEBUG

private:
	HRESULT Add_Components(TRIGGERDESC* pTriggerDesc);

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END