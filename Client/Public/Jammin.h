#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CJammin final : public CGameObject
{
public:
	typedef struct tagElfInitDesc
	{
		_float4x4 WorldMatrix;
		wstring wstrAnimationTag = { TEXT("") };
#ifdef _DEBUG
		_bool isCheckPosition = { false };
#endif // _DEBUG
	}ELFINITDESC;

private:
	explicit CJammin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CJammin(const CJammin& rhs);
	virtual ~CJammin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

#ifdef _DEBUG
private:
	_bool m_isCheckPosition = { false };
	_float3 m_vAngle;
	void Tick_TestShake();
#endif // _DEBUG

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	static CJammin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END