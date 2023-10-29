#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
class CModel_Instance;
END

BEGIN(Client)

class CCrowd final : public CGameObject
{
private:
	explicit CCrowd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCrowd(const CCrowd& rhs);
	virtual ~CCrowd() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint m_iNumCrowds = { 0 };
	_float4x4* m_WorldMatrices = { nullptr };

private:
	CModel_Instance* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CCrowd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END