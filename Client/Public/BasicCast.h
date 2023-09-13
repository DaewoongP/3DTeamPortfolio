#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"
#include "MeshEffect.h"

BEGIN(Client)
class CBasicCast final : public CMagicBall
{
private:
	explicit CBasicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBasicCast(const CBasicCast& rhs);
	virtual ~CBasicCast() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;

private:
	CMeshEffect*		 m_pMeshEffect = { nullptr };
	CTransform*			 m_pMeshEffectTrans = { nullptr };

private:
	_float3				m_vTargetPosition = {};
	_float				m_fLerpAcc = { 0.f };

private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CBasicCast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END