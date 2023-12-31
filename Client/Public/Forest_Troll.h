#pragma once
/* =============================================== */
//	[CForest_Troll]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)
class CWeapon_Forest_Troll;
END

BEGIN(Client)

class CForest_Troll final : public CGameObject
{
private:
	explicit CForest_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CForest_Troll(const CForest_Troll& rhs);
	virtual ~CForest_Troll() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

private:
	CWeapon_Forest_Troll* m_pWeapon = { nullptr };

private:
	PxScene* m_pScene = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CForest_Troll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END