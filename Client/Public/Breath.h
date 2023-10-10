#pragma once

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CBreath final : public CGameObject
{
private:
	explicit CBreath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBreath(const CBreath& rhs);
	virtual ~CBreath() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShader = { nullptr };
	CModel* m_pModel = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_Components_Level(const _uint& iLevel);
	HRESULT SetUp_ShaderResources();

public:
	static CBreath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
