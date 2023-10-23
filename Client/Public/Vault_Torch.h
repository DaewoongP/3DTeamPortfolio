#pragma once
#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CParticleSystem;
class CLight;
END

BEGIN(Client)

class CVault_Torch final : public CMapObject
{
private:
	explicit CVault_Torch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVault_Torch(const CVault_Torch& rhs);
	virtual ~CVault_Torch() = default;

public:
	_uint Get_TorchIndex() const { return m_iTorchIndex; }
	void Switch_OnOff(_bool isValue);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;

private:
	CParticleSystem* m_pEffect = { nullptr };
	CLight* m_pLight = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

private:
	// Torch 넘버링. 구역별로 1, 2, 3 이렇게 나눠진다.
	// 각 구역에는 총 12개의 Torch가 있다.
	_uint m_iTorchIndex = { 0 }; 
	_bool	m_isSwitch = { false };

private:
	void Torch_Bright();

public:
	static CVault_Torch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END