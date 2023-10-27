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
class CBroom_Stick_DarkWizard;
END

BEGIN(Client)

class CFlyMan final : public CGameObject
{
public:
	typedef struct tagFlyManInitDesc
	{
		_float4x4 WorldMatrix;
		wstring wstrAnimationTag = { TEXT("") };
#ifdef _DEBUG
		_bool isCheckPosition = { false };
#endif // _DEBUG
	}FLYMANINITDESC;

private:
	explicit CFlyMan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFlyMan(const CFlyMan& rhs);
	virtual ~CFlyMan() = default;

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

	CBroom_Stick_DarkWizard* m_pBroom_Stick = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	static CFlyMan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END