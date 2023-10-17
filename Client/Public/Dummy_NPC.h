#pragma once
#include "GameObject.h"
#include "CustomModel.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
END

BEGIN(Client)

class CDummy_NPC final : public CGameObject
{
public:
	typedef struct tagNPCInitDesc
	{
		_float4x4 WorldMatrix;
		wstring wstrCustomModelTag = { TEXT("") };
		// 0 : HAT, 1 : HAIR, 2 : HEAD, 3 : MASK, 4 : ARM, 
		// 5 : ROBE, 6 : TOP, 7 : PANTS, 8 : SOCKS, 9 : SHOES
		array<wstring, CCustomModel::MESH_END> MeshPartsTags;
		wstring wstrAnimationTag = { TEXT("") };
	}NPCINITDESC;

private:
	explicit CDummy_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_NPC(const CDummy_NPC& rhs);
	virtual ~CDummy_NPC() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	array<wstring, CCustomModel::MESH_END> m_MeshPartTags;

private:
	CCustomModel* m_pCustomModel = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };

private:
	HRESULT Add_Components(const NPCINITDESC& Desc);
	HRESULT Ready_MeshParts(const NPCINITDESC& Desc);
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	static CDummy_NPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END