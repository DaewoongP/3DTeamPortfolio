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

class CHouse_Elf final : public CGameObject
{
public:
	typedef struct tagElfInitDesc
	{
		_float4x4 WorldMatrix;
		wstring wstrAnimationTag = { TEXT("") };
		_float4 vClothColor;
	}ELFINITDESC;

private:
	explicit CHouse_Elf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHouse_Elf(const CHouse_Elf& rhs);
	virtual ~CHouse_Elf() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	_float m_fTimeDelta = { 0.f };
	_bool m_isWalk = { false };
	_bool m_isTurn = { false };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

private:
	void Walk(const _float& fTimeDelta);

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	static CHouse_Elf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END