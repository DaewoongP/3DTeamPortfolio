#pragma once

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)
class CCylinder;
END

BEGIN(Client)

class CBreath final : public CGameObject
{
public:
	typedef struct tagBreathInitDesc
	{
		_float3 vPosition;
		_float3 vRight;
		_float fRadian = { 0.f };
		_float fDistance = { 0.f };
	}BREATHINITDESC;

private:
	explicit CBreath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBreath(const CBreath& rhs);
	virtual ~CBreath() = default;

public:
	void On_Breath() {
		m_isActionBreath = true;
	}
	void Off_Breath(){
		m_isActionBreath = false;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

public:
	void Reset(const BREATHINITDESC& tagResetDesc);

private:
	_bool m_isActionBreath = { false };

private:
	CCylinder* m_pCylinder = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShader = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBreath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END